#pragma once
#include "AttendanceRules.h"
namespace attendance::context {
inline float smooth(float v){v=(std::clamp)(v,0.0f,1.0f);return v*v*(3-2*v);}
inline float quality(int rank){return rank>0?std::exp(-(rank-1)/35.0f):0;}
inline float separation(const State& raw){
    State s=ranked(raw);if(s.rank0<1||s.rank1<1)return 0;
    // Log ranks distinguish a top-end mismatch from the same gap lower down.
    float gap=std::abs(std::log((s.rank0+8.0f)/(s.rank1+8.0f)));
    return gap/(gap+1.4f);
}
inline float importance(const State& raw){
    State s=ranked(raw);float a=quality(s.rank0),b=quality(s.rank1);
    float opponent=s.neutral?(std::max)(a,b):b;
    float value=0.6f*std::sqrt(a*b)+0.4f*opponent;
    if(top25(s.rank0)&&top25(s.rank1))value=(std::max)(value,0.85f);
    if(s.playoff)value=(std::max)(value,championship(s)?1.0f:0.95f);
    return value;
}
inline float variation(const State& s){
    if(!s.variationSeed)return 0;
    unsigned x=s.variationSeed;x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;
    return static_cast<float>(x&65535)/32767.5f-1;
}
inline float comparableHomeWin(const State& raw){
    State s=ranked(raw);
    if(s.neutral||s.homeScore<=s.awayScore||s.rank0<1||s.rank1<1)return 0;
    return (1-smooth(relativeGap(s)))*(1-inferiorOpponent(s));
}
struct HalftimeTarget {float permanent=0,temporary=0;};
inline HalftimeTarget halftime(const State& s){
    int margin=std::abs(s.homeScore-s.awayScore);
    float wet=badWeather(s)?0.15f:0;
    float patience=!s.neutral&&s.homeScore>s.awayScore?0.30f-0.15f*inferiorOpponent(s):0;
    float choose=(std::clamp)(0.65f+wet-patience+0.08f*variation(s),0.0f,1.0f);
    const float temporary=20+10*choose;
    if(s.playoff)return {1+3*choose,temporary};
    bool special=top25(s.rank0)&&top25(s.rank1);
    if(margin<=8)return special?HalftimeTarget{1+3*choose,temporary}:HalftimeTarget{5+5*choose,temporary};
    if(margin<=16){float t=(margin-8)/8.0f;return {7.5f+2.5f*t,temporary};}
    float patienceFactor=1-0.25f*comparableHomeWin(s);
    if(margin<=24){float t=(margin-17)/7.0f;return {patienceFactor*(std::clamp)(10+10*t+2*(choose-0.5f),10.0f,20.0f),temporary};}
    if(margin<=34){float t=(margin-25)/9.0f;return {patienceFactor*(std::clamp)(20+10*t+3*(choose-0.5f),20.0f,30.0f),temporary};}
    float t=(std::clamp)((margin-35)/14.0f,0.0f,1.0f);
    return {patienceFactor*(30+10*(0.4f*t+0.6f*choose)),temporary};
}
// Read-only, verified possession context. Unknown values leave the score/time
// model in charge; neutral sites remain symmetric without attendance splits.
inline float lateOpportunity(const State& s){
    float window=s.quarterSeconds*.25f;
    if(s.neutral||s.quarter!=4||s.seconds>window||s.homeScore>=s.awayScore)return 1;
    if(s.offense<0||s.offense>1||s.homeTimeouts<0||s.homeTimeouts>3)return 1;
    int margin=s.awayScore-s.homeScore;
    float late=smooth((window-s.seconds)/(window*(150.f/180.f))),multi=smooth((margin-8)/9.0f);
    if(s.offense==0){
        float red=s.toGoal>=0&&s.toGoal<=20?0.12f:0;
        return 1-late*(0.15f+red)*(1-multi*0.5f);
    }
    return 1+late*multi*(0.20f+0.10f*(3-s.homeTimeouts));
}
}
