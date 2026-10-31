#pragma once
#include "SustainedCrowd.h"
#include "DenseCrowd.h"
namespace attendance::background {
// Reuse the verified native sustained channel at low priority, not a timer
// repeatedly firing cheer samples. Reactions take ownership of the same voice.
inline sustained::Intent merge(sustained::Intent out,const State& s,uint64_t positions,float fullness){
    if(!out.active||out.reason!=sustained::Reason::None||!valid(s)||
       (s.phase!=1&&s.phase!=3&&s.phase!=4&&s.phase!=5)||
       positions<512||positions>12800000||!std::isfinite(fullness)||fullness<.60f||fullness>1)return out;
    double occupied=positions*static_cast<double>(fullness);if(occupied<10000)return out;
    auto tiers=dense::layers(positions,fullness);
    float weight=dense::smooth((fullness-.60f)/.20f)*dense::smooth(static_cast<float>((occupied-10000)/14000));
    if(weight<=0)return out;
    out.reason=sustained::Reason::Background;out.priority=0;out.continuous=true;out.reaction=false;
    out.size=tiers.veryLarge>.5f?3:tiers.full>.5f?2:1;
    out.intensity=(std::max)(out.intensity,weight*(350+200*tiers.full+100*tiers.veryLarge));
    out.cheers=false;out.supporting=false;out.attackMs=4000;out.releaseMs=2000;
    return out;
}
}
