#pragma once
#include "DirectorShake.h"
#include "SustainedCrowd.h"

namespace attendance::pairing {
// Sound follows the selected camera envelope, never the rendered zoom strength.
// No new cheer, gain bonus, play event or timer is created by this bridge.
inline sustained::Intent merge(sustained::Intent out,const motion::Cue& cue,uint64_t now,float fullness,
                              uint64_t sequence,bool visibleBuildup=false,bool earnedAllowed=true){
    if(!sustained::fresh(out,cue.match,cue.epoch,now)||!cue.match||out.match!=cue.match||out.epoch!=cue.epoch||cue.kind==motion::Kind::None||motion::limit(cue.percent)<=0||
       now<cue.updated||now-cue.updated>750||now<cue.began||!std::isfinite(fullness)||fullness<=0||fullness>1)return out;
    float peak=(std::max)(motion::limit(cue.percent),motion::limit(cue.impact));if(peak<=0)return out;
    bool buildup=cue.kind==motion::Kind::Ordinary||cue.kind==motion::Kind::Critical;
    bool voiced=out.reason!=sustained::Reason::None&&(out.continuous||out.reaction);
    if(out.celebration||out.reason==sustained::Reason::Eruption)return out;
    if(buildup){
        if(voiced||!visibleBuildup)return out;
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
    if(out.reason==sustained::Reason::Result&&out.priority>4){
        // Preserve the official scoring/result response, extending only its
        // hold/release when a custom camera profile would otherwise outlive it.
        out.releaseMs=(std::max)(out.releaseMs,release);
        if(!releasing){out.continuous=true;out.reaction=false;out.supporting=true;}
        return out;
    }
    if(out.reason==sustained::Reason::Result&&!cue.impactOnly)return out; // Existing run/return pair already owns its fade.
    out.reason=sustained::Reason::Result;out.priority=(std::max)(out.priority,4);
    out.reactionId=sequence?sequence:cue.began;out.continuous=!releasing;out.reaction=releasing;
    out.size=cue.softAccent?2:3;out.supporting=true;out.supportingOnly=cue.softAccent;out.motionCompanion=true;
    out.attackMs=cue.softAccent?450:cue.impactOnly?100:500;out.releaseMs=release;
    return out;
}
}
