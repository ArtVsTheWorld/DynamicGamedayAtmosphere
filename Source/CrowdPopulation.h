#pragma once
#include "SustainedCrowd.h"
#include "VenueMotion.h"
namespace attendance::population {
inline double occupied(uint64_t positions,float fullness){
    return positions>=512&&positions<=12800000&&std::isfinite(fullness)?positions*static_cast<double>((std::clamp)(fullness,0.f,1.f)):0;
}
inline float presence(uint64_t positions,float fullness){
    return static_cast<float>(std::sqrt((std::clamp)(occupied(positions,fullness)/packed::LargePositions,0.,1.)));
}
inline int size(uint64_t positions,float fullness){double count=occupied(positions,fullness);return count<12000?1:count<36000?2:3;}
inline bool reactionFamily(int type){int family=type/4;return type>=0&&(family==2||family==14||family==18||family==20||family==22||family==23||family==25);}
inline bool negative(int type){return type>=0&&(type/4==23||type/4==25);}
inline int reactionType(int type,int sizeCode){return reactionFamily(type)?type/4*4+(std::clamp)(sizeCode,1,3):type;}
// Replace the existing density-presence factor for DGA's added layer intent.
// Native excitement/cheer peaks, background quieting and volume caps remain
// with their existing owners. Apply once, before final-play intent capture.
inline sustained::Intent layers(sustained::Intent out,uint64_t positions,float fullness){
    if(!out.active||out.reason==sustained::Reason::Diagnostics||out.reason==sustained::Reason::Postgame)return out;
    out.populationSize=size(positions,fullness);out.size=(std::min)(out.size,out.populationSize);
    if(out.reason==sustained::Reason::None)return out;
    if(!std::isfinite(fullness)||fullness<=0||!std::isfinite(out.intensity)){out.intensity=0;return out;}
    float previous=std::sqrt((std::clamp)(fullness,0.f,1.f));
    out.intensity=(std::clamp)(out.intensity*presence(positions,fullness)/previous,0.f,1000.f);
    return out;
}
}
