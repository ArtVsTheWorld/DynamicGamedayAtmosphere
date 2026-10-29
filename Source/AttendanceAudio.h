#pragma once
#include "AudioLevels.h"
#include <array>
#include <cstdint>
namespace attendance::balance {
// Retained only for the earned-reaction envelope model, never stacked as a
// separate bus-volume bonus by the production audio callback.
inline float crowdBoost(float f,float maximum=25){return std::isfinite(f)&&std::isfinite(maximum)?(std::clamp)(maximum,0.f,25.f)*(std::clamp)((f-.75f)/.25f,0.f,1.f):0;}
inline bool audiblePhase(int phase,bool unpaused,bool replay){return !replay&&(phase==6||phase==7||(unpaused&&phase>=1&&phase<=5));}

inline constexpr std::array<unsigned short,8> Groups={9,12,13,14,15,16,17,18};
inline constexpr std::array<const char*,8> Names={"Crowd","Anticipation_Home_Pass","Beds","Chants/Response","Reactions","StudentSection","Sweeteners","Swells"};
inline float smooth(float x){x=(std::clamp)(x,0.f,1.f);return x*x*(3-2*x);}
inline float quarter(float x){return std::round(x*4)*.25f;}
inline float venueWeight(uint64_t seats){return (std::clamp)(std::log(static_cast<float>((std::max)(seats,uint64_t(6060)))/6060.f)/std::log(61888.f/6060.f),0.f,1.f);}
inline float crossover(uint64_t seats){return .70f-.40f*venueWeight(seats);}
struct Mix {
 std::array<float,8> gain{};std::array<int,9> key{};bool valid=false,mute=false;
 float net(unsigned child)const{return gain[0]+gain[child+1];}
 bool needed()const{if(mute)return true;for(float db:gain)if(db!=0)return true;return false;}
};
// Fullness and mapped occupied positions determine density and the crossover.
// Sparse anchors are listening choices, not measured acoustic statistics.
inline Mix mix(float fullness,uint64_t seats,float maximumPercent=30,float strengthPercent=100){
 Mix out;if(!std::isfinite(fullness)||fullness<0||seats<512||seats>12800000||!std::isfinite(maximumPercent)||!std::isfinite(strengthPercent))return out;
 out.valid=true;fullness=(std::min)(fullness,1.f);maximumPercent=(std::clamp)(maximumPercent,0.f,30.f);float cap=volume::boostDb(maximumPercent);
 if(fullness==0){out.mute=true;out.gain[0]=-60;}
 else {
  constexpr std::array<float,7> low={-11.75f,-9.5f,-9.5f,-7.75f,-17.5f,.75f,-19.f};
  constexpr std::array<float,7> mid={-5.75f,-8.25f,-8.25f,-4.75f,-8.25f,-2.25f,-8.75f};
  constexpr std::array<float,7> powers={2,2.8f,2.8f,1.6f,2.8f,.8f,3};
  constexpr float a=3.f/7,b=5.f/7;std::array<float,7> net{};float point=crossover(seats);
  if(fullness<point){
   float r=fullness/point;
   for(unsigned i=0;i<7;++i){
    if(r<=a)net[i]=low[i]+20*powers[i]*std::log10(r/a);
    else if(r<b)net[i]=low[i]+(mid[i]-low[i])*smooth((r-a)/(b-a));
    else net[i]=mid[i]*(1-smooth((r-b)/(1-b)));
   }
  }else {float lift=(std::min)(maximumPercent*.01f,.15f+.15f*venueWeight(seats));float db=20*std::log10(1+lift*smooth((fullness-point)/(1-point)));net.fill(db);}
  // Quantize small changes but enforce the configured positive cap exactly.
  for(auto& db:net){db*=(std::clamp)(strengthPercent,0.f,100.f)*.01f;db=db>=cap-.00001f?cap:(std::clamp)(quarter(db),-60.f,cap);out.gain[0]=(std::max)(out.gain[0],db);}
  for(unsigned i=0;i<7;++i)out.gain[i+1]=net[i]-out.gain[0];
 }
 for(unsigned i=0;i<8;++i)out.key[i]=static_cast<int>(std::lround(out.gain[i]*10000));
 out.key[8]=out.mute?1:0;return out;
}
}
