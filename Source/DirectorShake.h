#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include "ReactionShape.h"

namespace attendance::motion {
enum class Kind { None,Ordinary,Critical,Touchdown,Eruption,LivePlay,Safety,BlockedKick };
inline const char* name(Kind k){switch(k){case Kind::BlockedKick:return "blocked-field-goal impact";case Kind::Ordinary:return "ordinary buildup";case Kind::Critical:return "critical buildup";case Kind::Touchdown:return "touchdown";case Kind::Eruption:return "eruption";case Kind::LivePlay:return "live-play reaction";case Kind::Safety:return "safety";default:return "inactive";}}
struct Settings {float ordinary=5,critical=10,touchdown=15,eruption=22.5f,live=10,catchImpact=8;float touchdownImpact=20,eruptionImpact=30;unsigned touchdownMs=11000,eruptionMs=16000;float lineupCheer=5;unsigned reactionFadeMs=5000,impactExtraMs=1000;float hostileOpeningMaximum=15;};
inline float hostileOpeningStrength(uint64_t positions,float maximum=15){
    if(positions<512||positions>12800000||!std::isfinite(maximum))return 0;
    float size=static_cast<float>((std::clamp)((static_cast<double>(positions)-6060)/55828.0,0.0,1.0));
    return (std::clamp)(maximum,0.f,30.f)*(1.f/3+2.f/3*size);
}
inline float limit(float x){return std::isfinite(x)?(std::clamp)(x,0.0f,100.0f):0;}
inline float configured(const Settings& s,Kind k){switch(k){case Kind::Ordinary:return limit(s.ordinary);case Kind::Critical:return limit(s.critical);case Kind::Touchdown:case Kind::Safety:case Kind::BlockedKick:return limit(s.touchdown);case Kind::Eruption:return limit(s.eruption);case Kind::LivePlay:return limit(s.live);default:return 0;}}
inline unsigned safetyDuration(const Settings& s){return (std::clamp)(s.touchdownMs,3000u,30000u)*7/10;}
inline float lineupStrength(const Settings& s,bool critical,float choice){
    float maximum=configured(s,critical?Kind::Critical:Kind::Ordinary),minimum=(std::min)(maximum,critical?5.0f:3.0f);
    float fraction=std::isfinite(choice)?(std::clamp)(choice,0.0f,1.0f):0;
    return minimum+(maximum-minimum)*fraction;
}
struct Cue {
    uintptr_t match=0;unsigned epoch=0;uint64_t updated=0,began=0,until=0;
    Kind kind=Kind::None;float percent=0;bool tail=false;float impact=0;uint64_t oscillationBegan=0;bool impactOnly=false;
    float transitionFrom=0;uint64_t transitionAt=0;bool carryPlay=false;
    bool runBuildup=false;
    bool catchReaction=false;
    unsigned distanceExtraMs=0;
    float hostileOpening=0;
    bool confirmedRun=false;
};
inline float smooth(float x){x=(std::clamp)(x,0.0f,1.0f);return x*x*(3-2*x);}
inline float envelopeStrength(const Cue& c,uint64_t now){
    if(c.kind==Kind::None||!c.match||now<c.updated||now-c.updated>750||now<c.began||!std::isfinite(c.percent))return 0;
    float age=static_cast<float>(now-c.began);
    if(c.impactOnly){if(now>=c.until||c.until<=c.began+400)return 0;return limit(c.percent)*shape::impact(age,static_cast<float>(c.until-c.began));}
    if(c.tail){unsigned total=c.until>c.began?static_cast<unsigned>((std::min)(c.until-c.began,uint64_t{16000})):0;return limit(c.percent)*shape::tail(age,static_cast<float>(total),c.confirmedRun?static_cast<float>(shape::tailHold(total)):0.f);}
    if(c.kind==Kind::Ordinary||c.kind==Kind::Critical||c.kind==Kind::LivePlay)return limit(c.percent)*smooth(age/(c.kind==Kind::LivePlay?200.0f:c.kind==Kind::Critical?600.0f:1000.0f));
    if(now>=c.until||c.until<=c.began||limit(c.percent)<=0)return 0;
    float duration=static_cast<float>(c.until-c.began),baseDuration=(std::max)(1.f,duration-c.distanceExtraMs),progress=age/baseDuration;
    bool td=c.kind==Kind::Touchdown||c.kind==Kind::Safety||c.kind==Kind::BlockedKick;
    float peakEnd=td?1.0f/9:1.25f/16,settleEnd=td?1.5f/9:2.0f/16,holdEnd=td?5.0f/9:8.0f/16;
    float peak=(std::max)(limit(c.percent),limit(c.impact));
    if(progress<peakEnd)return peak*smooth(age/(td?80.0f:60.0f));
    if(progress<settleEnd)return peak+(limit(c.percent)-peak)*smooth((progress-peakEnd)/(settleEnd-peakEnd));
    if(progress<holdEnd)return limit(c.percent);
    return limit(c.percent)*smooth((duration-age)/(duration-baseDuration*holdEnd));
}
inline float strength(const Cue& c,uint64_t now){
    float level=envelopeStrength(c,now);
    if(c.transitionAt&&now>=c.transitionAt&&now-c.transitionAt<200)return c.transitionFrom+(level-c.transitionFrom)*smooth(static_cast<float>(now-c.transitionAt)/200);
    return level;
}
struct Input {
    uintptr_t match=0;unsigned epoch=0;uint64_t now=0;int phase=0;
    bool allowed=false,lineup=false,critical=false,eruptionAlive=false;
    bool venueEligible=false,duringAllowed=true,openingKickoff=false;
    uint64_t lineupId=0,touchdown=0,eruption=0,duringPlay=0;
    float lineupScale=1;bool lineupCheer=false,duringImpact=false,duringCatch=false;
    uint64_t runBuildup=0;float runProgress=0;
    uint64_t safety=0;
    unsigned distanceExtraMs=0;
    float hostileOpening=0;uint64_t blockedFieldGoal=0;
};
class Model {
    uintptr_t match_=0;unsigned epoch_=0;uint64_t last_=0,lineup_=0,seenTd_=0,seenEruption_=0,seenLive_=0,ramp_=0,seenSafety_=0,seenBlockedKick_=0;int phase_=0;
    Cue lineupCue_,pulse_,liveCue_,output_;
public:
    void reset(){*this=Model{};}
    Cue suspend(uint64_t now){lineup_=ramp_=0;lineupCue_=pulse_=liveCue_={};output_={};output_.updated=now;return output_;}
    Cue observe(const Settings& settings,const Input& x){
        unsigned reactionFade=(std::clamp)(settings.reactionFadeMs,1000u,8000u);
        if(x.match!=match_||x.epoch!=epoch_){reset();match_=x.match;epoch_=x.epoch;}
        bool td=x.touchdown&&x.touchdown!=seenTd_,eruption=x.eruption&&x.eruption!=seenEruption_,safety=x.safety&&x.safety!=seenSafety_;
        bool blocked=x.blockedFieldGoal&&x.blockedFieldGoal!=seenBlockedKick_;if(x.blockedFieldGoal)seenBlockedKick_=x.blockedFieldGoal;
        bool live=x.duringPlay&&x.duringPlay!=seenLive_;if(x.duringPlay)seenLive_=x.duringPlay;
        if(x.touchdown)seenTd_=x.touchdown;if(x.eruption)seenEruption_=x.eruption;if(x.safety)seenSafety_=x.safety;
        if(!x.allowed||!x.match||x.now<last_){last_=x.now;return suspend(x.now);}last_=x.now;
        bool newSnap=x.phase==3&&phase_!=3;phase_=x.phase;
        Cue previous=output_;float previousLevel=strength(previous,x.now);
        auto join=[&](Cue& cue){if(previousLevel>0){cue.oscillationBegan=previous.oscillationBegan?previous.oscillationBegan:previous.began;cue.transitionFrom=previousLevel;cue.transitionAt=x.now;}};
        if(x.lineup&&x.venueEligible&&x.lineupId&&(x.phase==1||x.phase==5)){
            if(lineup_!=x.lineupId||lineupCue_.tail||lineupCue_.kind==Kind::None){lineupCue_={x.match,x.epoch,x.now,x.now,0,Kind::Ordinary,0,false};lineup_=x.lineupId;lineupCue_.oscillationBegan=x.now;}
            float prior=strength(lineupCue_,x.now),target=x.hostileOpening>0?limit(x.hostileOpening):x.lineupCheer&&!x.critical?limit(settings.lineupCheer):lineupStrength(settings,x.critical,x.lineupScale);
            if(std::fabs(target-lineupCue_.percent)>.01f){lineupCue_.transitionFrom=prior;lineupCue_.transitionAt=x.now;}
            lineupCue_.kind=x.critical?Kind::Critical:Kind::Ordinary;
            lineupCue_.percent=target;lineupCue_.hostileOpening=x.hostileOpening;lineupCue_.updated=x.now;
            lineupCue_.carryPlay=x.openingKickoff;
        }else if(lineupCue_.kind!=Kind::None){
            bool carry=lineupCue_.carryPlay&&x.phase==3&&x.now>=lineupCue_.began&&x.now-lineupCue_.began<45000;
            if(!lineupCue_.tail&&!carry){float value=strength(lineupCue_,x.now);lineupCue_.began=x.now;lineupCue_.until=x.now+(lineupCue_.carryPlay?2000:900);lineupCue_.percent=value;lineupCue_.tail=true;lineup_=0;}
            lineupCue_.updated=x.now;if(lineupCue_.tail&&x.now>=lineupCue_.until)lineupCue_={};
        }
        // A presentation cut or play-selection inset is not a new play.
        // Retain the earned impact/tail until its original expiry; a fresh
        // snap, pause/replay suspension or world change cancels the old cue.
        if(newSnap){liveCue_={};ramp_=0;}
        if(!x.duringAllowed&&liveCue_.kind!=Kind::None&&!liveCue_.tail&&!liveCue_.impactOnly){
            float value=strength(liveCue_,x.now);liveCue_.began=x.now;liveCue_.until=x.now+reactionFade;liveCue_.percent=value;liveCue_.tail=true;liveCue_.confirmedRun=false;
        }
        bool ramp=x.runBuildup&&x.duringAllowed&&x.phase==3&&std::isfinite(x.runProgress)&&x.runProgress>0;
        if(ramp&&(liveCue_.kind==Kind::None||ramp_!=x.runBuildup)){liveCue_={x.match,x.epoch,x.now,x.now,x.now+45000,Kind::LivePlay,0,false,0,x.now,false};liveCue_.runBuildup=true;ramp_=x.runBuildup;join(liveCue_);}
        // Crossing the configured gain confirms the cheer without restarting
        // the shake that builds over the preceding five yards.
        if(live&&x.duringAllowed&&!(ramp&&liveCue_.kind==Kind::LivePlay&&!liveCue_.tail)&&(x.phase==3||x.phase==4||((x.phase==5||x.phase==1)&&x.duringImpact))){bool impact=x.phase!=3||x.duringImpact;liveCue_={x.match,x.epoch,x.now,x.now,x.now+(impact?reactionFade+400+(std::min)(settings.impactExtraMs,5000u):45000),Kind::LivePlay,impact&&x.duringCatch?limit(settings.catchImpact):configured(settings,Kind::LivePlay),false,0,x.now,impact};liveCue_.catchReaction=x.duringCatch;join(liveCue_);}
        if(live&&x.duringAllowed&&liveCue_.kind==Kind::LivePlay)liveCue_.confirmedRun=true;
        if(liveCue_.kind!=Kind::None){
            if(!liveCue_.tail&&!liveCue_.impactOnly)liveCue_.distanceExtraMs=(std::max)(liveCue_.distanceExtraMs,(std::min)(x.distanceExtraMs,8000u));
            if(liveCue_.catchReaction&&!liveCue_.impactOnly&&x.phase!=3&&x.now>=liveCue_.began&&x.now-liveCue_.began<=350){
                liveCue_.impactOnly=true;liveCue_.percent=limit(settings.catchImpact);liveCue_.until=liveCue_.began+reactionFade+400+(std::min)(settings.impactExtraMs,5000u);join(liveCue_);
            }
            if(liveCue_.runBuildup&&!liveCue_.confirmedRun&&!ramp&&x.phase==3&&!liveCue_.tail){float value=strength(liveCue_,x.now);liveCue_.began=x.now;liveCue_.until=x.now+reactionFade+liveCue_.distanceExtraMs;liveCue_.percent=value;liveCue_.tail=true;}
            if(!liveCue_.impactOnly&&x.phase!=3&&!liveCue_.tail){float value=(std::max)(strength(liveCue_,x.now),liveCue_.percent*.7f);liveCue_.began=x.now;liveCue_.until=x.now+reactionFade+liveCue_.distanceExtraMs;liveCue_.percent=value;liveCue_.tail=true;}
            liveCue_.updated=x.now;if(!liveCue_.tail)liveCue_.percent=liveCue_.impactOnly&&liveCue_.catchReaction?limit(settings.catchImpact):ramp?(std::min)(configured(settings,Kind::LivePlay),1.f+(configured(settings,Kind::LivePlay)-1.f)*(std::clamp)((x.runProgress-.0001f)/.9999f,0.f,1.f)):configured(settings,Kind::LivePlay);
            if(x.now>=liveCue_.until)liveCue_={};
        }
        if(pulse_.kind!=Kind::None&&(x.now>=pulse_.until||newSnap||(pulse_.kind==Kind::Eruption&&!x.eruptionAlive)))pulse_={};
        if(eruption&&x.phase!=3){
            bool upgrade=(pulse_.kind==Kind::Touchdown||pulse_.kind==Kind::Safety)&&x.now>=pulse_.began&&x.now-pulse_.began<750;
            auto start=upgrade?pulse_.began:x.now;pulse_={x.match,x.epoch,x.now,start,start+(std::clamp)(settings.eruptionMs,5000u,60000u),Kind::Eruption,configured(settings,Kind::Eruption),false,limit(settings.eruptionImpact)};
            pulse_.distanceExtraMs=(std::min)(x.distanceExtraMs,8000u);pulse_.until+=pulse_.distanceExtraMs;
            join(pulse_);liveCue_={};ramp_=0;
        }else if(td&&x.phase!=3&&pulse_.kind!=Kind::Eruption){pulse_={x.match,x.epoch,x.now,x.now,x.now+(std::clamp)(settings.touchdownMs,3000u,30000u),Kind::Touchdown,configured(settings,Kind::Touchdown),false,limit(settings.touchdownImpact)};pulse_.distanceExtraMs=(std::min)(x.distanceExtraMs,8000u);pulse_.until+=pulse_.distanceExtraMs;join(pulse_);liveCue_={};ramp_=0;}
        else if(blocked&&x.phase!=3&&pulse_.kind!=Kind::Eruption&&pulse_.kind!=Kind::Touchdown){pulse_={x.match,x.epoch,x.now,x.now,x.now+(std::clamp)(settings.touchdownMs,3000u,30000u),Kind::BlockedKick,configured(settings,Kind::BlockedKick),false,limit(settings.touchdownImpact)};join(pulse_);liveCue_={};ramp_=0;}
        else if(safety&&x.phase!=3&&pulse_.kind!=Kind::Eruption){pulse_={x.match,x.epoch,x.now,x.now,x.now+safetyDuration(settings),Kind::Safety,configured(settings,Kind::Safety),false,limit(settings.touchdownImpact)};join(pulse_);liveCue_={};ramp_=0;}
        if(pulse_.kind!=Kind::None){pulse_.updated=x.now;pulse_.percent=configured(settings,pulse_.kind);pulse_.impact=limit(pulse_.kind==Kind::Eruption?settings.eruptionImpact:settings.touchdownImpact);output_=pulse_;}
        else if(liveCue_.kind!=Kind::None)output_=liveCue_;else{
            if(lineupCue_.kind!=Kind::None&&previous.kind!=Kind::None&&previous.kind!=Kind::Ordinary&&previous.kind!=Kind::Critical){lineupCue_.transitionFrom=previousLevel;lineupCue_.transitionAt=x.now;}
            output_=lineupCue_;
        }
        return output_;
    }
};
inline bool validMatrix(const std::array<float,16>& m){
    for(float v:m)if(!std::isfinite(v)||std::fabs(v)>100000)return false;
    for(int row=0;row<3;++row){float norm=0;for(int col=0;col<3;++col)norm+=m[row*4+col]*m[row*4+col];if(std::fabs(norm-1)>.02f||std::fabs(m[row*4+3])>.01f)return false;}
    for(int a=0;a<3;++a)for(int b=a+1;b<3;++b){float dot=0;for(int i=0;i<3;++i)dot+=m[a*4+i]*m[b*4+i];if(std::fabs(dot)>.02f)return false;}
    return std::fabs(m[15]-1)<.01f;
}
inline void rotate(std::array<float,16>& m,int a,int b,float radians){
    float c=std::cos(radians),s=std::sin(radians);for(int row=0;row<4;++row){float x=m[row*4+a],y=m[row*4+b];m[row*4+a]=c*x+s*y;m[row*4+b]=-s*x+c*y;}
}
inline float projectionScale(const std::array<float,16>& p,Kind kind){
    if(kind==Kind::None)return 1;
    // The native projection getter exposes this separate perspective matrix.
    // Focal length magnifies angular motion; compensate without changing zoom.
    bool valid=true;for(float v:p)valid=valid&&std::isfinite(v);
    valid=valid&&p[0]>.1f&&p[0]<10000&&p[5]>.1f&&p[5]<10000&&std::fabs(p[15])<.001f&&std::fabs(std::fabs(p[11])-1)<.01f;
    if(!valid)return .35f;
    constexpr float reference=3.1715948f; // 35-degree vertical field of view.
    return (std::clamp)(reference/p[5],0.0f,1.0f);
}
inline bool apply(std::array<float,16>& output,const Cue& c,uint64_t now){
    float amount=strength(c,now)*.01f;if(amount<=0||!validMatrix(output))return false;
    constexpr float pi=3.14159265358979323846f;
    float t=static_cast<float>((now-(c.oscillationBegan?c.oscillationBegan:c.began))%1000000)*.001f;auto result=output;
    rotate(result,1,2,.65f*(pi/180)*std::sin(2*pi*7*t)*amount);
    rotate(result,0,2,.45f*(pi/180)*std::sin(2*pi*9*t+1)*amount);
    rotate(result,0,1,.12f*(pi/180)*std::sin(2*pi*5*t)*amount);
    if(!validMatrix(result))return false;output=result;return true;
}
}
