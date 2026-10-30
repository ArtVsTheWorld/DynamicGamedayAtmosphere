#pragma once
#include "ReactionPalette.h"
#include "NativeSkillSignals.h"
namespace attendance::awaysound {
struct Outcome {int type=0;bool scoring=false;};
// Reuse the exact trackers over the same verified samples, with symmetric
// classification only. These proofs NEVER feed energy, attendance or motion.
class Model {
 energy::Tracker tracker_;live::Model live_;skills::Model skills_;energy::Outcome result_;
 uint64_t sequence_=0;bool emitted_=false,scored_=false;
public:
 void reset(){*this=Model{};}
 // Main situational reactions can arrive before the symmetric gain threshold.
 // Cover that play across ticks, while retaining a later verified score upgrade.
 void cover(uint64_t sequence,bool scoring=false){if(sequence_!=sequence){sequence_=sequence;emitted_=scored_=false;}emitted_=true;scored_=scored_||scoring;}
 void track(const energy::Settings& c,const energy::Play& p,uint64_t mainSequence){auto symmetric=p;symmetric.game.neutral=true;if(p.game.phase==6){tracker_.primeOpeningIntro(symmetric);result_={};}else result_=tracker_.observe(c,symmetric);if(result_.accepted)result_.sequence=mainSequence;}
 Outcome observe(const live::Settings& c,const energy::Play& p,uintptr_t match,unsigned epoch,uint64_t sequence,uint64_t now,bool allowed,
  uintptr_t record,int kind,int ball,float catchSpot,float runnerSpot,bool held,bool skillAllowed,const skills::Sample& sample){
  if(sequence_!=sequence){sequence_=sequence;emitted_=scored_=false;}
  auto symmetric=p;symmetric.game.neutral=true;
  auto event=live_.observe(c,symmetric,match,epoch,now,allowed,record,kind,ball,catchSpot,runnerSpot,held,false,&result_);
  auto skill=skills_.observe(symmetric,match,epoch,now,allowed&&skillAllowed,held,sample);if(!event.id)event=skill;
  if(!allowed||p.game.neutral||!sequence||p.simulated||energy::clockManagementContext(p,tracker_.start,sequence))return {};
  bool result=result_.accepted&&result_.completed&&result_.sequence==sequence&&result_.side==1;
  bool scoring=result&&(result_.awayPoints>=6||result_.awayPoints==2||result_.awayPoints==3);
  if(result&&palette::positiveOutcome(result_,tracker_.start)&&(!emitted_||(scoring&&!scored_))){
   emitted_=true;scored_=scored_||scoring;return {palette::resultType(result_,tracker_.start,p.game,tracker_.sawPassReleased,sequence,epoch,true),scoring};
  }
  if(!emitted_&&event.id&&event.side==1){emitted_=true;return {palette::liveType(event,tracker_.start,p.game,sequence,epoch,true),false};}
  return {};
 }
};
}
