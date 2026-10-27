#pragma once
#include "SustainedCrowd.h"
#include "ReactionShape.h"

namespace attendance::soundhandoff {
// Carry an already-earned crowd level through a confirmed score's attack.
// This never creates an event, restarts a voice or changes gain/timer limits.
class Model {
    uintptr_t match_=0;unsigned epoch_=0;uint64_t last_=0,began_=0,score_=0;
    bool eruption_=false,voiced_=false;float prior_=0,floor_=0;
public:
    void reset(){*this=Model{};}
    sustained::Intent merge(sustained::Intent out,int phase,uint64_t now,float fullness){
        if(out.match!=match_||out.epoch!=epoch_){reset();match_=out.match;epoch_=out.epoch;}
        if(!out.active||!sustained::fresh(out,out.match,out.epoch,now)||!std::isfinite(fullness)||fullness<=0||fullness>1||
           now<last_||(last_&&now-last_>750)){reset();return out;}
        float presence=std::sqrt(fullness);
        bool eruption=out.reason==sustained::Reason::Eruption&&out.celebration;
        bool scoring=eruption||(out.scoringReaction&&out.reason==sustained::Reason::Result&&out.reactionId);
        uint64_t id=eruption?out.celebration:out.reactionId;
        if(phase==3)began_=0;
        if(scoring&&(id!=score_||eruption!=eruption_)){
            began_=phase!=3&&voiced_&&last_&&now-last_<=250?now:0;
            floor_=prior_;score_=id;eruption_=eruption;
        }
        if(scoring&&began_&&now>=began_&&now-began_<500){
            auto age=now-began_;
            float hold=age<=250?1.f:1-shape::smooth(static_cast<float>(age-250)/250.f);
            out.intensity=(std::max)(out.intensity,floor_*presence*hold);
        }
        voiced_=(out.reason==sustained::Reason::Result||out.reason==sustained::Reason::Eruption)&&
            (out.continuous||out.reaction)&&std::isfinite(out.intensity)&&out.intensity>0;
        prior_=voiced_?(std::clamp)(out.intensity/presence,0.f,1000.f):0;last_=now;
        return out;
    }
};
}
