#pragma once
#include "DirectorShake.h"
#include "SustainedCrowd.h"

namespace attendance::pairing {
struct SoundLease {uintptr_t match=0;unsigned epoch=0;uint64_t updated=0;bool audible=false;uint64_t audibleAt=0;};
inline float soundWeight(const SoundLease& sound,const motion::Cue& cue,uint64_t now,bool presentationCarry=false){
    if(!sound.match||sound.match!=cue.match||sound.epoch!=cue.epoch||now<sound.updated)return 0;
    bool earned=cue.kind!=motion::Kind::None&&cue.kind!=motion::Kind::Ordinary&&cue.kind!=motion::Kind::Critical;
    uint64_t heard=sound.audible?sound.updated:sound.audibleAt;
    if(!heard||now<heard)return 0;
    auto age=now-heard;
    // A timeout/penalty may stop the native voice while an earned camera
    // envelope is still finishing. Preserve only a previously heard cue in a
    // fresh owned sound context; this does not create or extend its lifetime.
    if(presentationCarry&&earned&&heard>=cue.began&&now-sound.updated<=750)return 1;
    if(age<=150)return 1;
    // Brief native voice/camera handoffs fade rather than hard-gating an
    // already earned impact on/off. No new sound lease is manufactured.
    return earned&&age<300?1-shape::smooth(static_cast<float>(age-150)/150.f):0;
}
inline bool soundReady(const SoundLease& sound,const motion::Cue& cue,uint64_t now){
    return soundWeight(sound,cue,now)>0;
}
// Sound follows the selected camera envelope, never the rendered zoom strength.
// No new cheer, gain bonus, play event or timer is created by this bridge.
inline sustained::Intent merge(sustained::Intent out,const motion::Cue& cue,uint64_t now,float fullness,
                              uint64_t sequence,bool visibleBuildup=false,bool earnedAllowed=true){
    if(!sustained::fresh(out,cue.match,cue.epoch,now)||!cue.match||out.match!=cue.match||out.epoch!=cue.epoch||cue.kind==motion::Kind::None||motion::limit(cue.percent)<=0||
       now<cue.updated||now-cue.updated>750||now<cue.began||!std::isfinite(fullness)||fullness<=0||fullness>1)return out;
    float peak=(std::max)(motion::limit(cue.percent),motion::limit(cue.impact));if(peak<=0)return out;
    bool buildup=cue.kind==motion::Kind::Ordinary||cue.kind==motion::Kind::Critical;
    bool voiced=out.reason!=sustained::Reason::None&&(out.continuous||out.reaction)&&std::isfinite(out.intensity)&&out.intensity>0;
    if((out.celebration||out.reason==sustained::Reason::Eruption)&&voiced)return out;
    if(buildup){
        if(!visibleBuildup)return out;
        float level=650.f*std::sqrt(fullness)*(std::clamp)(motion::strength(cue,now)/peak,0.f,1.f);
        if(voiced||(out.reason!=sustained::Reason::None&&(out.continuous||out.reaction))){out.intensity=(std::max)(out.intensity,level);return out;}
        // The prepared camera may be hidden behind a presentation cutaway.
        // Only its actually rendered gameplay transition gets this prelude.
        out.reason=sustained::Reason::Lineup;out.priority=(std::max)(out.priority,1);
        out.lineup=cue.oscillationBegan?cue.oscillationBegan:cue.began;
        out.lineupBegan=cue.began;out.lineupUntil=now+750;
        out.continuous=!cue.tail;out.reaction=cue.tail;out.size=cue.hostileOpening>0||cue.kind==motion::Kind::Critical?3:2;
        out.supporting=true;out.supportingOnly=true;out.motionCompanion=true;
        out.attackMs=1000;out.releaseMs=1000;
        out.intensity=(std::max)(out.intensity,650.f*std::sqrt(fullness)*(std::clamp)(motion::strength(cue,now)/peak,0.f,1.f));
        return out;
    }
    if(!earnedAllowed||cue.cancelled)return out; // Never resurrect a cheer intentionally stopped after lost possession.
    unsigned total=cue.until>cue.began?static_cast<unsigned>((std::min)(cue.until-cue.began,uint64_t{90000})):0;
    if(!total||now>=cue.until)return out;
    float age=static_cast<float>(now-cue.began),hold=0;
    bool longLive=cue.kind==motion::Kind::LivePlay&&!cue.tail&&!cue.impactOnly;
    if(cue.impactOnly)hold=(std::min)(1000.f,total*.2f);
    else if(cue.tail)hold=cue.confirmedRun?static_cast<float>(shape::tailHold(total)):0;
    else if(!longLive)hold=total*(cue.kind==motion::Kind::Eruption?.5f:5.f/9);
    if(cue.softAccent)hold=450;
    bool releasing=!longLive&&age>=hold;
    int release=longLive?5000:static_cast<int>((std::max)(500.f,total-hold));
    float level=(std::clamp)(motion::strength(cue,now)/peak,0.f,1.f);
    out.intensity=(std::max)(out.intensity,(cue.softAccent?650.f:950.f)*std::sqrt(fullness)*level);
    if(out.celebration||out.reason==sustained::Reason::Eruption){
        // An eruption identity alone does not guarantee a playing voice.
        // Repair a silent/incomplete intent without inventing another cheer.
        out.reason=sustained::Reason::Eruption;out.priority=(std::max)(out.priority,6);
        out.continuous=!releasing;out.reaction=releasing;out.supporting=!releasing;
        out.size=3;out.attackMs=500;out.releaseMs=(std::max)(out.releaseMs,release);
        out.motionCompanion=true;return out;
    }
    if(out.reason==sustained::Reason::Result&&out.priority>4){
        // Preserve the official scoring/result response, extending only its
        // hold/release when a custom camera profile would otherwise outlive it.
        out.releaseMs=(std::max)(out.releaseMs,release);
        if(!releasing){out.continuous=true;out.reaction=false;out.supporting=true;}
        else if(!voiced){out.reaction=true;out.continuous=false;}
        return out;
    }
    if(out.reason==sustained::Reason::Result&&!cue.impactOnly)return out; // Existing run/return pair already owns its fade.
    out.reason=sustained::Reason::Result;out.priority=(std::max)(out.priority,4);
    out.earnedReaction=cue.kind!=motion::Kind::LivePlay||!cue.runBuildup||cue.confirmedRun;
    out.reactionHoldUntil=longLive?0:cue.began+static_cast<uint64_t>(hold);out.reactionUntil=longLive?0:cue.until;
    out.reactionId=sequence?sequence:cue.began;out.continuous=!releasing;out.reaction=releasing;
    out.size=cue.softAccent?2:3;out.supporting=true;out.supportingOnly=cue.softAccent;out.motionCompanion=true;
    out.attackMs=cue.softAccent?450:cue.impactOnly?100:500;out.releaseMs=release;
    return out;
}
}
