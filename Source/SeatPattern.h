#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <vector>
#include "SeatHash.h"
#include "CrowdVisibility.h"
namespace attendance::seats {
struct Position {float x=0,y=0,z=0,packed=0;};
static_assert(sizeof(Position)==16);
enum class Policy {Pending,Position,Scattered};
struct Pattern {
    Policy policy=Policy::Pending;unsigned original=0;std::vector<unsigned> rank;
    bool departed(unsigned agent,int loss)const{
        loss=(std::clamp)(loss,0,100);
        if(policy!=Policy::Position)return attendance::departed(agent,loss);
        return agent<rank.size()&&rank[agent]<static_cast<uint64_t>(original)*static_cast<unsigned>(loss)/100;
    }
};
inline double noise(double x,double z,double scale=28,int seed=0){
    double gx=x/scale,gz=z/scale;int ix=static_cast<int>(std::floor(gx)),iz=static_cast<int>(std::floor(gz));
    double tx=gx-ix,tz=gz-iz;tx=tx*tx*(3-2*tx);tz=tz*tz*(3-2*tz);
    auto value=[&](int a,int b){return seed?random({a,b,seed}):random({a,b});};
    double a=value(ix,iz),b=value(ix+1,iz),c=value(ix,iz+1),d=value(ix+1,iz+1);
    return (a+(b-a)*tx)*(1-tz)+(c+(d-c)*tx)*tz;
}
inline double interpolate(double p,const std::array<double,4>& values){
    if(p<=.125)return values[0];if(p>=.875)return values[3];
    int k=(std::min)(2,static_cast<int>((p-.125)/.25));double t=(p-(.125+k*.25))/.25;t=t*t*(3-2*t);
    return values[k]+(values[k+1]-values[k])*t;
}
inline double smooth(double x){x=(std::clamp)(x,0.0,1.0);return x*x*(3-2*x);}
// Keep the proven near-full picture exactly. Below 90%, progressively allow
// lower seating areas to thin while retaining fuller neighboring pockets.
inline double lowerBowlBlend(double missing){return smooth((missing-.10)/.40);}
inline double lowerBowlWeight(double depth){return 1-smooth((depth-.25)/.45);}
inline int32_t rounded(double x){
    double f=std::floor(x),t=x-f;return static_cast<int32_t>(f+(t>.5||(t==.5&&static_cast<int64_t>(f)%2!=0)?1:0));
}
inline double distance(const Position& p){
    double x=(std::max)(std::abs(static_cast<double>(p.x))-24.384,0.0),z=(std::max)(std::abs(static_cast<double>(p.z))-54.864,0.0);
    return std::sqrt(x*x+z*z+static_cast<double>(p.y)*p.y);
}
inline bool normalize(const std::vector<double>& weights,size_t target,std::vector<double>& probabilities){
    if(weights.empty()||!target||target>=weights.size())return false;
    double total=0;for(double w:weights){if(!std::isfinite(w)||w<=0)return false;total+=w;}
    double scale=static_cast<double>(target)/total;size_t previous=0;
    for(unsigned iteration=0;iteration<128;++iteration){
        size_t capped=0;double uncapped=0;
        for(double w:weights){if(scale*w>1)++capped;else uncapped+=w;}
        if(capped==previous){probabilities.resize(weights.size());for(size_t i=0;i<weights.size();++i)probabilities[i]=(std::min)(1.0,scale*weights[i]);return true;}
        if(capped<previous||capped>=target||uncapped<=0)return false;
        scale=static_cast<double>(target-capped)/uncapped;previous=capped;
    }
    return false;
}
// Reject unfamiliar geometry rather than infer a field transform from it.
// Multiple renderers may cover only part of the bowl, so symmetry is not assumed.
inline bool plausible(const std::vector<Position>& positions,const std::vector<unsigned>& ids){
    if(positions.size()<512||positions.size()>200000||ids.size()<256)return false;
    unsigned outside=0;double lowest=1e9,highest=-1e9;
    for(unsigned i:ids){if(i>=positions.size())return false;const auto& p=positions[i];
        if(!std::isfinite(p.x)||!std::isfinite(p.y)||!std::isfinite(p.z)||std::abs(p.x)>300||std::abs(p.z)>350||p.y<-.5f||p.y>150)return false;
        double d=distance(p);lowest=(std::min)(lowest,d);highest=(std::max)(highest,d);
        if(std::abs(p.x)>24.384||std::abs(p.z)>54.864)++outside;
    }
    return outside>=ids.size()*9/10&&highest-lowest>5&&highest<250;
}
inline Pattern build(const std::vector<Position>& positions,const std::vector<unsigned short>& baseline,unsigned short parts,unsigned seed=0){
    Pattern result;result.policy=Policy::Scattered;
    if(!parts||positions.size()!=baseline.size())return result;
    std::vector<unsigned> ids;for(unsigned i=0;i<baseline.size();++i)if((baseline[i]&parts)!=parts)ids.push_back(i);
    if(!plausible(positions,ids))return result;
    const size_t n=ids.size();auto byDepth=ids;
    std::sort(byDepth.begin(),byDepth.end(),[&](unsigned a,unsigned b){double da=distance(positions[a]),db=distance(positions[b]);return da==db?a<b:da<db;});
    std::vector<double> percentile(positions.size()),logWeight(positions.size()),area(positions.size()),choice(positions.size()),lower(positions.size()),pockets(positions.size());
    for(size_t k=0;k<n;++k)percentile[byDepth[k]]=(static_cast<double>(k)+.5)/static_cast<double>(n);
    for(unsigned i:ids){
        const auto& p=positions[i];double x=p.x,y=p.y,z=p.z,depth=percentile[i];
        // One continuous curve avoids density shoulders at seating-zone edges.
        logWeight[i]=std::log(1.45-.9*depth*depth*(3-2*depth));
        int fx=static_cast<int>(std::floor(x/1.8)),fy=static_cast<int>(std::floor(y)),fz=static_cast<int>(std::floor(z/1.8));
        int32_t rx=rounded(x*100),ry=rounded(y*100),rz=rounded(z*100);
        choice[i]=random({rx,ry,rz,117})<.32?random({rx,ry,rz,71}):random({fx,fy,fz});
        int sectionSeed=static_cast<int>(seed%100000);
        // Uneven arrival/exit order across the entire bowl, with soft-edged
        // seating areas, families and scattered stragglers still overlapping.
        area[i]=(.45*(2*noise(x,z,28,sectionSeed+11)-1)+.35*(2*noise(x+97,z-131,9,sectionSeed+27)-1)+
            .20*(2*noise(x-33,z+51,55,sectionSeed+59)-1))*(.25+.75*std::pow(depth,1.2));
        lower[i]=lowerBowlWeight(depth);
        // Spatial fields are fixed by the match seed. Keep the existing
        // family/straggler choices and feather patches across row boundaries.
        pockets[i]=.55*(2*noise(x+41,z-73,24,sectionSeed+101)-1)+
            .30*(2*noise(x-89,z+29,7,sectionSeed+137)-1)+
            .15*(2*noise(x+17,z+113,48,sectionSeed+173)-1);
    }
    std::vector<unsigned> remaining=ids,order;order.reserve(n);
    std::vector<double> horizontal(n),weights(n),probabilities,probability(positions.size()),score(positions.size());
    for(unsigned loss=1;loss<=100;++loss){
        size_t target=n-static_cast<uint64_t>(n)*loss/100;
        if(!target){
            std::sort(remaining.begin(),remaining.end(),[&](unsigned a,unsigned b){return score[a]==score[b]?a<b:score[a]<score[b];});
            order.insert(order.end(),remaining.begin(),remaining.end());break;
        }
        double missing=static_cast<double>(loss)/100,blend=lowerBowlBlend(missing);std::array<double,4> sums{};std::array<size_t,4> counts{};
        for(size_t k=0;k<n;++k){unsigned i=ids[k],zone=(std::min)(3u,static_cast<unsigned>(percentile[i]*4));
            double varied=area[i]+.65*blend*lower[i]*pockets[i];
            horizontal[k]=std::exp(4.4*missing*varied);sums[zone]+=horizontal[k];++counts[zone];}
        for(unsigned k=0;k<4;++k){if(!counts[k])return result;sums[k]/=static_cast<double>(counts[k]);}
        for(size_t k=0;k<n;++k){unsigned i=ids[k];weights[k]=std::exp(2.3*missing*(logWeight[i]-.40*blend*lower[i]))*horizontal[k]/interpolate(percentile[i],sums);}
        if(!normalize(weights,target,probabilities))return result;
        for(size_t k=0;k<n;++k)probability[ids[k]]=probabilities[k];
        for(unsigned i:remaining)score[i]=choice[i]/(std::max)(1e-15,probability[i]);
        auto compare=[&](unsigned a,unsigned b){return score[a]==score[b]?a<b:score[a]<score[b];};
        auto cut=remaining.begin()+static_cast<ptrdiff_t>(target);
        if(cut!=remaining.end())std::nth_element(remaining.begin(),cut,remaining.end(),compare);
        std::sort(cut,remaining.end(),compare);
        for(size_t k=remaining.size();k>target;--k)order.push_back(remaining[k-1]);
        remaining.resize(target);
    }
    if(order.size()!=n)return result;
    result.rank.assign(positions.size(),(std::numeric_limits<unsigned>::max)());
    for(unsigned k=0;k<order.size();++k)result.rank[order[k]]=k;
    result.original=static_cast<unsigned>(n);result.policy=Policy::Position;return result;
}
}
