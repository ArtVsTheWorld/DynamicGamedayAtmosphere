#pragma once
#include "AtmosphereRules.h"
#include "CrowdMomentum.h"
#include "AtmosphereMix.h"
#include "ScoringEnergy.h"
#include "OpeningAtmosphere.h"
#include <cstdint>
#include <limits>

namespace attendance::energy {
struct Settings {
    int enabled=1,eruptions=1,chaos=1,upsetWatch=1,lateScoreDepartures=5;
    float ordinaryDb=3,exceptionalDb=6,maxDb=6,decisiveDb=8;
    float bigGameDb=2,rankedRivalryDb=3,playoffDb=3,championshipDb=4,ambienceTargetDb=6;
    int opening=1;float openingCrowdDb=3,openingAmbienceDb=6;
    float baselineDb=0;int highScoring=1;
    int momentum=1,overtimeAtmosphere=1;float momentumHalfLife=45,overtimeDb=4;
    int sustained=1;
};
struct Play {
    State game;
    int down=-2,offense=-1,homeTimeouts=-1;
    float toGoal=-1,toFirst=-1;
    float fieldSpot=std::numeric_limits<float>::quiet_NaN();int attackDirection=0;
    uint64_t resultStamp=0;
    bool interception=false,firstDown=false;
};
enum class Event { None,FourthDown,GoalLine,PickSix,LateDrive,HomeScore,UpsetWin,
    OpeningReturn,OpeningTouchdown,LastSecondLead,Turnover,FourthStop,TakeLead,TieScore,LongTouchdown,BackfieldStop,ThirdStop,ExplosiveGain,OvertimeWin,ConversionStop,OvertimeConversion,ThirdDownConversion,GameWinningScore };
inline const char* name(Event e){
    switch(e){
    case Event::FourthDown:return "Fourth-down conversion";
    case Event::GoalLine:return "Goal-line stand";
    case Event::PickSix:return "Interception return touchdown";
    case Event::LateDrive:return "Late go-ahead score";
    case Event::HomeScore:return "Touchdown";
    case Event::UpsetWin:return "Major upset completed";
    case Event::OpeningReturn:return "Opening kickoff return touchdown";
    case Event::OpeningTouchdown:return "First offensive snap touchdown";
    case Event::LastSecondLead:return "Final-90-seconds go-ahead score";
    case Event::Turnover:return "Defensive interception";
    case Event::FourthStop:return "Fourth-down defensive stop";
    case Event::TakeLead:return "Lead-changing score";
    case Event::TieScore:return "Fourth-quarter tying score";
    case Event::LongTouchdown:return "Long touchdown";
    case Event::BackfieldStop:return "Backfield stop";
    case Event::ThirdStop:return "Third-down defensive stop";
    case Event::ExplosiveGain:return "10+ yard first down";
    case Event::ThirdDownConversion:return "Third-down conversion";
    case Event::OvertimeWin:return "Overtime winner";
    case Event::GameWinningScore:return "Game-winning score";
    case Event::ConversionStop:return "Overtime conversion stop";
    case Event::OvertimeConversion:return "Overtime two-point conversion";
    default:return "None";
    }
}
inline int priority(Event e){
    switch(e){
    case Event::OvertimeWin:return 100;case Event::GameWinningScore:return 99;case Event::LastSecondLead:return 95;case Event::PickSix:return 90;
    case Event::UpsetWin:return 88;case Event::OpeningReturn:return 85;case Event::OpeningTouchdown:return 82;
    case Event::OvertimeConversion:return 81;case Event::ConversionStop:return 78;
    case Event::LateDrive:return 80;case Event::Turnover:return 75;case Event::GoalLine:return 72;
    case Event::FourthStop:return 70;case Event::FourthDown:return 68;case Event::TakeLead:return 65;
    case Event::TieScore:return 62;case Event::LongTouchdown:return 60;case Event::HomeScore:return 55;
    case Event::ThirdDownConversion:return 50;case Event::ThirdStop:return 45;case Event::ExplosiveGain:return 40;case Event::BackfieldStop:return 35;
    default:return 0;
    }
}
inline Event preferred(Event a,Event b){return priority(b)>priority(a)?b:a;}
inline bool normalDown(const Play& p){return p.down>=1&&p.down<=4&&(p.offense==0||p.offense==1);}
// The native line of scrimmage is available even without a selected-player
// presentation context. Use the direction verified before the snap, since the
// first-down line can move behind the ball while the result is being presented.
inline float gainedYards(const Play& before,const Play& after){
    if(before.offense!=after.offense)return -1000;
    if(std::isfinite(before.fieldSpot)&&std::isfinite(after.fieldSpot)&&
       (before.attackDirection==1||before.attackDirection==-1)){
        float gain=(after.fieldSpot-before.fieldSpot)*before.attackDirection;
        if(std::fabs(gain)<=100.01f)return gain;
    }
    if(before.toGoal>=0&&after.toGoal>=0)return before.toGoal-after.toGoal;
    return -1000;
}
inline bool earnedFirstDown(const Play& before,const Play& after,bool sawFlag){
    if(!normalDown(before)||!normalDown(after)||before.offense!=after.offense||after.down!=1)return false;
    float gain=gainedYards(before,after);
    return sawFlag||(before.toFirst>0&&gain+0.01f>=before.toFirst);
}
inline bool overtimeTry(const Play& p){return overtime(p.game)&&p.down==-1&&(p.offense==0||p.offense==1);}
inline bool eligibleSnap(const Play& p){return normalDown(p)||(p.down==0&&(p.offense==0||p.offense==1))||overtimeTry(p);}
inline bool primaryScore(int delta){return delta==2||delta==3||(delta>=6&&delta<=8);}
inline bool lateExit(const Settings& c,const Play& before,const Play& after){
    const State& s=after.game;
    return c.lateScoreDepartures>0&&!s.neutral&&s.quarter==4&&s.seconds<=120&&s.seconds>=0&&!s.playoff&&
        before.down!=-1&&primaryScore(s.awayScore-before.game.awayScore)&&s.homeScore==before.game.homeScore&&
        s.awayScore-s.homeScore>=9&&after.homeTimeouts>=0&&after.homeTimeouts<=1;
}
struct Outcome {Event event=Event::None;bool lateDeparture=false,accepted=false,visitorScore=false;int side=0;bool completed=false,setback=false;uint64_t sequence=0;int homePoints=0,awayPoints=0;};
// Only a preplay -> live snap -> completed play sequence can produce events.
// Clock/score high-water marks remain after replay rejection. Attaching during
// a replay, a score correction, or Super Sim does not invent a live snap.
struct Tracker {
    Play start,ready;bool haveReady=false,inPlay=false,scoreUsed=false,resultUsed=false,seen=false,winUsed=false,played=false,sawFirstDown=false;
    int lastPhase=-1,highHome=0,highAway=0;
    float highTime=0;uint64_t startStamp=0;
    bool openingSeen=false,openingKickUsed=false,scrimmageUsed=false,openingKick=false,openingSnap=false,wasOvertime=false;
    uint64_t serial=0,lastResultStamp=0;
    void reset(){*this=Tracker{};}
    Outcome observe(const Settings& c,const Play& p){
        Outcome o;const State& s=p.game;if(!valid(s))return o;
        float t=elapsedGameTime(s);
        if(seen&&(t+0.01f<highTime||s.homeScore<highHome||s.awayScore<highAway)){
            haveReady=inPlay=false;lastPhase=-1;return o;
        }
        if(!seen)openingSeen=s.quarter==1&&s.homeScore==0&&s.awayScore==0&&s.seconds>=s.quarterSeconds-1&&(s.phase==1||s.phase==6);
        o.accepted=true;seen=true;highTime=t;highHome=s.homeScore;highAway=s.awayScore;
        State story=energyPerspective(s);
        wasOvertime=wasOvertime||overtime(s);
        if((c.eruptions||c.momentum)&&!winUsed&&played&&s.phase==7&&s.quarter>=4){
            if(c.overtimeAtmosphere&&wasOvertime&&s.homeScore!=s.awayScore&&(s.homeScore>s.awayScore||s.neutral)){
                o.event=Event::OvertimeWin;o.side=s.awayScore>s.homeScore?1:0;winUsed=true;
            }else if(story.homeScore>story.awayScore&&upsetDiscrepancy(s)>=0.3f){
                o.event=Event::UpsetWin;o.side=s.neutral&&s.rank1>s.rank0?1:0;winUsed=true;
            }
            if(o.event!=Event::None)o.sequence=serial;
        }
        if(s.phase==3&&lastPhase!=3){
            if(haveReady&&eligibleSnap(ready)&&ready.game.quarter==s.quarter&&
               ready.game.homeScore==s.homeScore&&ready.game.awayScore==s.awayScore){
                start=ready;startStamp=p.resultStamp;inPlay=played=true;scoreUsed=resultUsed=sawFirstDown=false;++serial;
                openingKick=openingSeen&&!openingKickUsed&&ready.down==0&&s.quarter==1&&s.homeScore==0&&s.awayScore==0&&s.seconds>=s.quarterSeconds-1;
                openingSnap=openingSeen&&!scrimmageUsed&&normalDown(ready)&&s.quarter==1&&s.homeScore==0&&s.awayScore==0;
                if(ready.down==0)openingKickUsed=true;
                if(normalDown(ready))scrimmageUsed=true;
            }else inPlay=false;
            haveReady=false;
        }
        bool finished=s.phase==4||s.phase==5||s.phase==1||s.phase==7;
        if(inPlay&&finished){
            sawFirstDown=sawFirstDown||p.firstDown;
            int home=s.homeScore-start.game.homeScore,away=s.awayScore-start.game.awayScore;
            if(!scoreUsed&&(primaryScore(home)||primaryScore(away))){
                o.lateDeparture=lateExit(c,start,p);scoreUsed=true;o.completed=true;o.sequence=serial;
                o.homePoints=(std::max)(0,home);o.awayPoints=(std::max)(0,away);
                o.visitorScore=!s.neutral&&primaryScore(away)&&home==0;
                int side=primaryScore(home)&&away==0?0:primaryScore(away)&&home==0?1:-1;
                if((c.eruptions||c.momentum)&&side>=0&&(side==0||s.neutral)){
                    State before=side==1?reversed(start.game):start.game,after=side==1?reversed(s):s;
                    int points=after.homeScore-before.homeScore;o.side=side;
                    bool ahead=before.homeScore<=before.awayScore&&after.homeScore>after.awayScore;
                    Event selected=points>=6?Event::HomeScore:overtimeTry(start)&&points==2?Event::OvertimeConversion:Event::None;
                    if(points>=6&&start.toGoal>=40&&start.offense==side)selected=preferred(selected,Event::LongTouchdown);
                    if(s.quarter==4&&before.homeScore<before.awayScore&&after.homeScore==after.awayScore)selected=preferred(selected,Event::TieScore);
                    if(ahead&&before.homeScore<before.awayScore)selected=preferred(selected,Event::TakeLead);
                    if(s.quarter==4&&s.seconds<=120&&ahead)selected=preferred(selected,Event::LateDrive);
                    if(points>=6&&start.offense!=side&&normalDown(start)&&p.interception&&p.resultStamp&&p.resultStamp!=startStamp)selected=preferred(selected,Event::PickSix);
                    if(points>=6&&openingSnap&&start.offense==side)selected=preferred(selected,Event::OpeningTouchdown);
                    if(points>=6&&openingKick)selected=preferred(selected,Event::OpeningReturn);
                    if(s.quarter==4&&s.seconds<=90&&ahead)selected=preferred(selected,Event::LastSecondLead);
                    if(s.phase==7&&start.game.quarter>=4&&ahead)selected=preferred(selected,Event::GameWinningScore);
                    o.event=preferred(o.event,selected);
                }
            }
            // Wait for the next live possession/down; postplay still contains
            // the old offense on some plays. Scores are already handled above.
            bool settled=s.phase==1||(s.phase==5&&(p.down!=start.down||p.offense!=start.offense));
            if(!resultUsed&&settled&&(normalDown(p)||overtimeTry(p))){
                bool offenseSupported=start.offense==0||s.neutral,defenseSupported=start.offense==1||s.neutral;
                bool fresh=!overtime(s)||(p.resultStamp&&p.resultStamp!=startStamp&&p.resultStamp!=lastResultStamp);
                if(!scoreUsed&&fresh){
                    o.completed=true;o.sequence=serial;
                    o.setback=!s.neutral&&((start.offense==0&&p.offense==1)||(start.offense==1&&p.offense==1&&p.down==1&&sawFirstDown));
                    if((c.eruptions||c.momentum)&&normalDown(start)){
                        if(offenseSupported&&earnedFirstDown(start,p,sawFirstDown)){
                            if(gainedYards(start,p)>=9.99f){o.event=preferred(o.event,Event::ExplosiveGain);o.side=start.offense;}
                            if(start.down==3){o.event=preferred(o.event,Event::ThirdDownConversion);o.side=start.offense;}
                            if(start.down==4){o.event=preferred(o.event,Event::FourthDown);o.side=start.offense;}
                        }
                        if(defenseSupported){
                            Event defensive=Event::None;
                            if(p.offense==start.offense&&p.down>start.down&&start.toFirst>=0&&p.toFirst>=start.toFirst+1.5f)
                                defensive=Event::BackfieldStop;
                            if(start.down==3&&((p.offense==start.offense&&p.down==4)||p.offense!=start.offense))defensive=preferred(defensive,Event::ThirdStop);
                            if(p.offense!=start.offense){
                                bool pick=p.interception&&p.resultStamp&&p.resultStamp!=startStamp;
                                if(start.down==4)defensive=preferred(defensive,Event::FourthStop);
                                if(pick)defensive=preferred(defensive,Event::Turnover);
                                if(start.toGoal>=0&&start.toGoal<=5&&(start.down==4||pick))defensive=preferred(defensive,Event::GoalLine);
                            }
                            if(priority(defensive)>priority(o.event)){o.event=defensive;o.side=1-start.offense;}
                        }
                    }
                    if((c.eruptions||c.momentum)&&overtimeTry(start)&&p.offense!=start.offense&&(start.offense==1||s.neutral)){
                        o.event=preferred(o.event,Event::ConversionStop);o.side=1-start.offense;
                    }
                    lastResultStamp=p.resultStamp;
                }
                resultUsed=true;inPlay=false;
            }
        }
        if(s.phase==1&&eligibleSnap(p)){ready=p;haveReady=true;}
        if(s.phase!=1&&s.phase!=3&&s.phase!=4&&s.phase!=5&&s.phase!=7){haveReady=inPlay=false;}
        lastPhase=s.phase;return o;
    }
};
inline float lateDriveAnticipation(const Settings& c,const Play& p){
    const State& s=p.game;
    if(!c.chaos||!normalDown(p)||(!s.neutral&&p.offense!=0)||s.quarter!=4||s.seconds>90||
       (s.phase!=1&&s.phase!=3)||p.toGoal<0||p.toGoal>20)return 0;
    int own=p.offense==1?s.awayScore:s.homeScore,other=p.offense==1?s.homeScore:s.awayScore;
    if(std::abs(own-other)>8)return 0;
    float proximity=(20-p.toGoal)/20;
    // A tying/go-ahead drive gets the full ramp; protecting an existing lead is quieter.
    return (25+25*proximity)*(own>other?0.55f:1.0f);
}
inline float anticipationPoints(const Settings& c,const Play& p,int stage){
    const State& s=p.game;float offensive=lateDriveAnticipation(c,p);
    if(c.chaos&&c.overtimeAtmosphere&&competitiveOvertime(s)&&(normalDown(p)||overtimeTry(p))&&(s.phase==1||s.phase==3)){
        float amount=s.neutral||p.offense==1?30.0f:20.0f;
        if(p.down==3)amount+=6;if(p.down==4||overtimeTry(p))amount+=12;
        if(p.toGoal>=0&&p.toGoal<=10)amount+=4;
        return (std::min)(46.0f,amount);
    }
    if(!c.chaos||!normalDown(p)||(!s.neutral&&p.offense!=1)||(p.down!=3&&p.down!=4)||
       (s.phase!=1&&s.phase!=3)||std::abs(s.homeScore-s.awayScore)>16)return offensive;
    float amount=p.down==4?24.0f:18.0f;
    if(s.quarter==4)amount+=10;
    if(p.toGoal>=0&&p.toGoal<=20)amount+=4;
    if(p.toFirst>=10)amount+=3; // Long defensive third downs invite a roar.
    if(std::abs(s.homeScore-s.awayScore)>8)amount*=0.6f;
    if(stage>=2)amount+=5;
    return (std::max)(offensive,(std::min)(40.0f,amount));
}
inline float eventPoints(Event e,const State& s,bool overtimeEffects=true){
    if(e==Event::None)return 0;
    // Once-in-a-game moments get a guaranteed eruption, even at an ordinary matchup.
    if(e==Event::OvertimeWin||e==Event::GameWinningScore)return 78;
    if(e==Event::LastSecondLead)return 72;
    if(e==Event::OpeningReturn||e==Event::OpeningTouchdown)return 64;
    float margin=static_cast<float>(std::abs(s.homeScore-s.awayScore));
    float closeness=margin<=8?1.0f:margin<=16?0.7f:margin<=24?0.35f:0.08f;
    if(e==Event::UpsetWin)closeness=(std::max)(0.75f,closeness);
    float importance=e==Event::ConversionStop?36.0f:e==Event::OvertimeConversion?48.0f:e==Event::BackfieldStop?20.0f:e==Event::ThirdStop?26.0f:e==Event::ExplosiveGain?26.0f:e==Event::ThirdDownConversion?32.0f:e==Event::HomeScore?22.0f:e==Event::FourthDown?38.0f:e==Event::GoalLine?36.0f:
        e==Event::Turnover||e==Event::FourthStop?30.0f:e==Event::TakeLead||e==Event::TieScore?36.0f:48.0f;
    float late=s.quarter==4?(s.seconds<=120?1.3f:1.15f):1.0f;
    float opponent=1+(top25(s.rank1)?(s.rank1<=10?0.15f:0.08f):0)+0.25f*upsetDiscrepancy(s);
    float extra=overtimeEffects&&overtime(s)&&(e==Event::Turnover||e==Event::FourthStop||e==Event::ThirdStop||e==Event::GoalLine||e==Event::PickSix||e==Event::ConversionStop)?1.6f:1.0f;
    return (std::min)(72.0f,importance*closeness*late*opponent*extra);
}
struct Envelope {
    float peak=0;double began=-1000,lastEvent=-1000;Event type=Event::None;uint64_t sequence=0;bool special=false;
    bool trigger(Event e,double now,const State& s,uint64_t play=0,bool overtimeEffects=true){
        if(e==Event::None||!std::isfinite(now))return false;
        float desired=eventPoints(e,s,overtimeEffects);
        bool same=play&&play==sequence;
        if(special&&now-began<16&&priority(e)<priority(type))return false;
        if(same&&priority(e)<=priority(type))return false;
        bool decisive=(e==Event::LastSecondLead||e==Event::OvertimeWin||e==Event::GameWinningScore)&&!same;
        if(now-lastEvent<8&&!decisive&&priority(e)<=priority(type))return false;
        peak=(std::max)(desired,value(now));began=lastEvent=now;type=e;sequence=play;
        special=e==Event::OvertimeWin||e==Event::GameWinningScore||(e==Event::LastSecondLead&&s.quarter==4&&s.seconds<=60);
        return true;
    }
    float specialWeight(double now)const{
        double age=now-began;
        if(!special||age<0||age>=16)return 0;
        if(age<0.75)return static_cast<float>(age/0.75);
        return age<=5?1.0f:static_cast<float>((16-age)/11);
    }
    float value(double now)const{
        double age=now-began;
        if(age<0||age>=16)return 0;
        if(age<0.75)return peak*static_cast<float>(age/0.75);
        if(age<=5)return peak;
        return peak*static_cast<float>((16-age)/11);
    }
};
struct Model {
    float score=20,anticipation=0,matchupFloor=0,heldFraction=1;int stage=0;double last=-1;Envelope surge;Momentum momentum;MomentumClock momentumClock;ScoringPace scoring;OpeningProfile openingProfile;bool overtimeActive=false;
    float ambienceLift=0,scoringLift=0,victoryDb=0,openingWeight=0;double victoryAt=-1;
    void reset(){*this=Model{};}
    bool event(Event e,double now,const State& s){return surge.trigger(e,now,s);}
    void setback(){surge=Envelope{};anticipation=0;momentum.setback();score=(std::max)(10.0f,score-12);}
    bool react(const Settings& c,const Outcome& result,double now,const State& s){
        if(!c.enabled||!result.accepted)return false;
        if(c.eruptions&&victoryAt<0&&s.phase==7&&(result.event==Event::OvertimeWin||result.event==Event::GameWinningScore||result.event==Event::LastSecondLead||result.event==Event::UpsetWin)){
            victoryDb=gainDb(c);victoryAt=now;
        }
        if(c.highScoring&&result.completed&&(result.homePoints>0||result.awayPoints>0))scoring.scored(s,result.homePoints+result.awayPoints);
        double momentumNow=momentumClock.advance(s,now);
        if(result.completed&&c.momentum){
            float points=result.event==Event::None?0:(std::min)(16.0f,eventPoints(result.event,s,c.overtimeAtmosphere!=0)*0.35f);
            momentum.observe(points,momentumNow,result.sequence,result.side,c.momentumHalfLife);
        }
        return c.eruptions&&result.event!=Event::None&&surge.trigger(result.event,now,s,result.sequence,c.overtimeAtmosphere!=0);
    }
    float update(const Settings& c,const Play& p,double now,float retainedFraction,float permanentFraction=-1){
        if(!valid(p.game)||!std::isfinite(now)||!std::isfinite(retainedFraction))return score;
        const State& s=p.game;
        double dt=last<0?0:(std::clamp)(now-last,0.0,0.25);last=now;
        heldFraction=(std::clamp)(retainedFraction,0.0f,1.0f);
        bool ranked=top25(s.rank0)&&top25(s.rank1);
        float identity=championship(s)?c.championshipDb:s.playoff?c.playoffDb:s.rivalry&&ranked?c.rankedRivalryDb:(s.rivalry||ranked)?c.bigGameDb:0;
        bool bigIdentity=s.rivalry||ranked||s.playoff;
        openingWeight=openingProfile.observe(s,p.offense,p.down,c.opening!=0,bigIdentity);
        float floor=identity;
        overtimeActive=c.overtimeAtmosphere&&competitiveOvertime(s);
        if(overtimeActive)floor=(std::max)(floor,c.overtimeDb);
        if(c.momentum)momentum.advance(momentumClock.advance(s,now),c.momentumHalfLife);else {momentum.reset();momentumClock.advance(s,now);}
        int gap=std::abs(s.homeScore-s.awayScore);
        // Big-game identity is present from kickoff, independent of a lucky
        // play. Close opening/final quarters retain the strongest atmosphere.
        float competitive=gap<=8?1.0f:gap<=16?0.7f:gap<=24?0.3f:0.0f;
        float period=(s.quarter==1||s.quarter==4||s.quarter>4)?1.0f:0.85f;
        float attendanceFactor=competitive*period*std::sqrt(heldFraction);
        matchupFloor=(std::clamp)(floor,0.0f,mix::NormalMaximumDb)*attendanceFactor;
        // This is a total Beds target, including the shared parent gain.
        ambienceLift=bigIdentity?(std::clamp)(c.ambienceTargetDb,0.0f,6.0f)*attendanceFactor:0;
        float permanent=permanentFraction<0?heldFraction:(std::clamp)(permanentFraction,0.0f,1.0f);
        if(openingWeight>0){
            float hype=openingWeight*competitive*std::sqrt(permanent);
            matchupFloor=(std::max)(matchupFloor,(std::clamp)(c.openingCrowdDb,0.0f,6.0f)*hype);
            ambienceLift=(std::max)(ambienceLift,(std::clamp)(c.openingAmbienceDb,0.0f,6.0f)*hype);
        }
        scoring.advance(s);scoringLift=c.highScoring?scoring.value(s):0;
        stage=c.upsetWatch?upsetStage(s,stage):0;
        float targetAnticipation=s.phase==3?anticipation:anticipationPoints(c,p,stage);
        if(c.chaos&&openingWeight>0&&normalDown(p)&&(s.neutral||p.offense==1)&&(s.phase==1||s.phase==3))
            targetAnticipation=(std::max)(targetAnticipation,24.0f*openingWeight*competitive);
        float step=static_cast<float>(dt)*(targetAnticipation>anticipation?7.5f:16.0f);
        anticipation+=(std::clamp)(targetAnticipation-anticipation,-step,step);
        int margin=std::abs(s.homeScore-s.awayScore);
        float baseline=overtimeActive?62.0f:20+(margin<=8?(s.quarter==4?22.0f:14.0f):margin<=16?8.0f:0.0f);
        if(s.rivalry)baseline+=12;
        if(ranked)baseline+=10;
        if(s.playoff)baseline+=8;
        if(stage)baseline+=(stage==1?6.0f:stage==2?14.0f:26.0f)*(0.4f+0.6f*upsetDiscrepancy(s));
        baseline=(std::min)(70.0f,baseline);
        float target=(std::clamp)(baseline+momentum.heat+scoringLift+(std::max)(anticipation,surge.value(now)),0.0f,100.0f);
        target*=0.5f+0.5f*std::sqrt((std::clamp)(retainedFraction,0.0f,1.0f));
        float rate=target>score?(surge.value(now)>anticipation?45.0f:8.0f):8.0f;
        step=static_cast<float>(dt)*rate;score+=(std::clamp)(target-score,-step,step);
        if(!c.enabled)score=0;
        return score;
    }
    float ceilingDb(const Settings& c,double now=-1)const{
        float normal=mix::normalCeiling(c.maxDb);if(normal<=0||!c.enabled)return 0;
        float decisive=(std::max)(normal,mix::ceiling(c.decisiveDb));
        return normal+(decisive-normal)*(c.eruptions?surge.specialWeight(now<0?last:now):0);
    }
    float gainDb(const Settings& c)const{
        if(!c.enabled||heldFraction<=0)return 0;
        // One shared output: matchup atmosphere plus the extra excitement
        // above a lively baseline. Stacked events cannot escape the ceiling.
        float dynamic=score<=40?0:score<=80?c.ordinaryDb*(score-40)/40:c.ordinaryDb+(c.exceptionalDb-c.ordinaryDb)*(score-80)/20;
        // A big game's sustained floor already supplies its ordinary energy;
        // only higher excitement adds to it, avoiding two baseline boosts.
        float db=matchupFloor>0?matchupFloor+(std::max)(0.0f,dynamic-c.ordinaryDb*0.45f):dynamic;
        db=(std::max)((std::clamp)(c.baselineDb,0.0f,6.0f),db);
        // Carry the outgoing level while the winner envelope ramps in. Then
        // ease that protection away rather than dropping the OT floor for a frame.
        double age=last-victoryAt;
        if(victoryAt>=0&&age>=0&&age<10){
            float weight=age<=4?1.0f:static_cast<float>((10-age)/6);
            db=(std::max)(db,victoryDb*weight);
        }
        float normal=mix::normalCeiling(c.maxDb),limit=ceilingDb(c);
        return (std::clamp)((std::min)(db,normal)+(limit-normal),0.0f,limit);
    }
};
}

