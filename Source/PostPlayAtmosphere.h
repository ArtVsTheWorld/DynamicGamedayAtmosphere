#pragma once
#include "ExcitementRules.h"
#include "ReactionShape.h"

namespace attendance::postplay {
struct Profile {float strength=0;double hold=0,release=0;int size=2;bool layered=false;};
inline Profile choose(const energy::Outcome& result,const State& s){
    using energy::Event;
    float strength=.45f;
    switch(result.event){
    case Event::ExplosiveGain:strength=result.yards>=40?.85f:result.yards>=20?.70f:.55f;break;
    case Event::ThirdDownConversion:case Event::ThirdStop:strength=.72f;break;
    case Event::FourthDown:case Event::FourthStop:strength=.90f;break;
    case Event::BackfieldStop:strength=.62f;break;
    case Event::Sack:strength=.72f;break;
    case Event::MadeFieldGoal:strength=.65f;break;
    case Event::ForcedFumble:case Event::BlockedFieldGoal:strength=.90f;break;
    case Event::Turnover:case Event::FumbleRecovery:case Event::GoalLine:strength=.95f;break;
    case Event::HomeScore:case Event::LongTouchdown:case Event::TakeLead:case Event::TieScore:case Event::PickSix:case Event::Safety:strength=.92f;break;
    default:break;
    }
    if(result.safety)strength=.92f;
    if(result.fiveYardLoss)strength=(std::max)(strength,.72f);
    if(result.yards>=20)strength=(std::max)(strength,result.yards>=40?.85f:.70f);
    if(result.highLeverageStop)strength=(std::max)(strength,.95f);
    int margin=std::abs(s.homeScore-s.awayScore);
    // Use the pre-score margin when available: a close touchdown can make the
    // scoreboard look less competitive than the play that caused the cheer.
    if(result.preSnapMargin>=0)margin=result.preSnapMargin;
    float context=margin<=8?1.0f:margin<=16?.90f:margin<=24?.72f:.50f;
    if(s.quarter>=4&&margin<=8)strength+=.06f;
    strength=(std::clamp)(strength*context,0.0f,1.0f);
    double duration=result.safety||result.event==Event::Safety?.70:1.0;
    return {strength,(4+4*strength)*duration,(6+3*strength)*duration,strength>=.68f?3:2,strength>=.85f};
}
inline float envelope(double age,const Profile& profile){
    return shape::response(age,profile.hold,profile.release,profile.strength);
}
}
