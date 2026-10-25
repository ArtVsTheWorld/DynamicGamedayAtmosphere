#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace attendance::quiet {
struct Intent {
    uintptr_t match=0;unsigned epoch=0;uint64_t updated=0;bool active=false;
    float fullness=1,strength=1,start=.50f;bool presentation=false;uint64_t positions=0;
};
inline bool fresh(const Intent& value,uintptr_t match,unsigned epoch,uint64_t now){
    return value.active&&match&&value.match==match&&value.epoch==epoch&&now>=value.updated&&now-value.updated<=750&&
        std::isfinite(value.fullness)&&std::isfinite(value.strength)&&std::isfinite(value.start);
}
inline bool audiblePhase(int phase,bool unpaused,bool interactiveReplay){
    if(interactiveReplay)return false;
    // Intros/final presentation can report the world as paused while their
    // own audio continues. Match ownership is checked separately by the caller.
    return phase==6||phase==7||(unpaused&&(phase==1||phase==3||phase==4||phase==5));
}
inline bool knownSize(uint64_t positions){return positions>=512&&positions<=12800000;}
inline float sizeWeight(uint64_t positions){
    if(!knownSize(positions))return 0;
    float x=static_cast<float>((std::clamp)((static_cast<double>(positions)-10000)/51888.0,0.0,1.0));
    return x*x*(3-2*x);
}
inline float ceiling(uint64_t positions){return knownSize(positions)?99+901*sizeWeight(positions):1001;}
inline float cap(float intensity,uint64_t positions){
    if(!knownSize(positions))return intensity;
    float maximum=ceiling(positions),soft=.25f*(1-sizeWeight(positions)),knee=maximum*(1-soft);
    if(intensity<=knee)return intensity;
    if(soft<=.000001f)return (std::min)(intensity,maximum);
    return knee+(maximum-knee)*(-std::expm1(-(intensity-knee)/(maximum-knee)));
}
inline float target(const Intent& value){
    float start=(std::clamp)(value.start,.01f,.50f);
    float fraction=(std::clamp)(value.fullness/start,0.0f,1.0f);
    // Below the configured fullness threshold, select progressively calmer voices.
    // This selects calmer native voices; it is not a percentage-volume fader.
    float base=std::pow(fraction,1.5f);
    // A sparse large bowl can still hold thousands of rendered fan positions.
    // Ease quieting with size AND actual occupied count; zero fans adds no lift.
    float people=knownSize(value.positions)?std::sqrt((std::clamp)(value.fullness*static_cast<float>(value.positions)/10000.f,0.f,1.f)):0;
    float adjusted=base+.35f*sizeWeight(value.positions)*people*(1-base);
    return 1-(std::clamp)(value.strength,0.0f,1.0f)*(1-adjusted);
}
inline float apply(float native,float factor){
    // The same native-intensity correction follows ambience AND native peaks.
    // No high-excitement exemption: a sparse crowd can react without returning
    // to full-stadium intensity. This is not a mute or a volume percentage.
    float x=(std::clamp)((native-350.0f)/650.0f,0.0f,1.0f);
    float peak=x*x*(3-2*x);
    // Preserve the tested ordinary bed. Trim native peaks progressively more
    // in sparse crowds, with up to 35% extra reduction at the lowest presence.
    return native*factor*(1-.35f*(1-factor)*peak);
}
class Fade {
    float value_=1,target_=1,left_=0;
public:
    Fade()=default;
    explicit Fade(float initial){reset(initial);}
    void reset(float value=1){value_=target_=value;left_=0;}
    float advance(float desired,float seconds){
        if(desired!=target_){target_=desired;left_=6;}
        if(seconds>=left_){value_=target_;left_=0;}
        else if(seconds>0){value_+=(target_-value_)*seconds/left_;left_-=seconds;}
        return value_;
    }
};
}
