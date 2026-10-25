#pragma once
#include "AudioLevels.h"

namespace attendance::balance {
inline float crowdBoost(float fraction,float maximum=25){
    if(!std::isfinite(fraction)||!std::isfinite(maximum))return 0;
    return (std::clamp)(maximum,0.0f,25.0f)*(std::clamp)((fraction-.75f)/.25f,0.0f,1.0f);
}
inline float chantBoost(float fraction,float maximum=15){
    if(!std::isfinite(fraction)||!std::isfinite(maximum))return 0;
    return (std::clamp)(maximum,0.0f,15.0f)*(std::clamp)((fraction-.75f)/.25f,0.0f,1.0f);
}
struct Mix {float crowd=0,chant=0;int crowdKey=0,chantKey=0;};
inline Mix mixer(float crowdPercent,float chantPercent){
    // Round toward the cap. The chant entry compensates the Crowd parent in
    // the same native preset, so its net boost stays independent of eruptions.
    int crowdKey=static_cast<int>(std::floor((std::clamp)(crowdPercent,0.0f,50.0f)+.0001f));
    int chantKey=static_cast<int>(std::floor((std::clamp)(chantPercent,0.0f,15.0f)*10+.0001f));
    float parent=volume::boostDb(static_cast<float>(crowdKey)),chant=volume::boostDb(chantKey*.1f);
    return {parent,chant-parent,crowdKey,chantKey};
}
}
