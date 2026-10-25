#pragma once
#include <algorithm>
#include <cmath>

namespace attendance::audio {
// A crowd-only linear gain stage. Do not apply to the game's Intensity control:
// the inspected CrowdLoopEntity uses a 0..1000 envelope, not a volume fader.
// Native audio binding is intentionally separate from this model.
class Gain {
    float current_=1.0f, target_=1.0f, retained_=1.0f, rampLeft_=0;
    void target(float next) noexcept {if(next!=target_){target_=next;rampLeft_=6.0f;}}
public:
    void reset() noexcept { current_=target_=retained_=1.0f;rampLeft_=0; }
    void restore() noexcept {retained_=1.0f;target(1.0f);} // Disable/end-of-game path only.
    void attendance(float remainingFraction,float maximumReduction=0.20f,float emphasis=1.0f,float arrivalFraction=1.0f,float reductionStart=0.80f) noexcept {
        if(!std::isfinite(remainingFraction)||!std::isfinite(maximumReduction)||!std::isfinite(emphasis)||!std::isfinite(arrivalFraction)||!std::isfinite(reductionStart))return;
        remainingFraction=(std::clamp)(remainingFraction,0.0f,1.0f);
        // The caller supplies fractions of the original attendance, not a
        // reduction to compound with the previously scaled audio output.
        retained_=(std::min)(retained_,remainingFraction);
        maximumReduction=(std::clamp)(maximumReduction,0.0f,1.0f);
        reductionStart=(std::clamp)(reductionStart,0.01f,1.0f);
        float present=(std::min)(retained_,(std::clamp)(arrivalFraction,0.0f,1.0f));
        // Percentage of native signal volume, independent of the dB boosts.
        // At default settings each ten attendance points below 80 costs 2.5%.
        float missing=(std::clamp)((reductionStart-present)/reductionStart,0.0f,1.0f);
        float level=present==0&&maximumReduction>0?0.0f:1-maximumReduction*missing;
        target((std::min)(1.0f,level*(std::clamp)(emphasis,1.0f,1.35f)));
    }
    float advance(float seconds) noexcept {
        if(!std::isfinite(seconds)||seconds<=0)return current_;
        // Every changed target eases over six seconds, even a small cut. Repeated
        // identical callback requests do not restart the fade or compound it.
        if(rampLeft_<=seconds){current_=target_;rampLeft_=0;}
        else {current_+=(target_-current_)*seconds/rampLeft_;rampLeft_-=seconds;}
        return current_;
    }
    float linear(float nativeValue) const noexcept {
        return std::isfinite(nativeValue)&&nativeValue>=0?nativeValue*current_:nativeValue;
    }
    float decibels(float nativeDb,float muteFloor=-96.0f) const noexcept {
        if(!std::isfinite(nativeDb)||!std::isfinite(muteFloor))return nativeDb;
        // Gain values downstream of the stock mixer graph are logarithmic:
        // add attenuation in dB; multiplying a negative dB value gets louder.
        if(current_==0)return (std::min)(nativeDb,muteFloor);
        return nativeDb+20.0f*std::log10(current_);
    }
    float value() const noexcept {return current_;}
    float target() const noexcept {return target_;}
};
}
