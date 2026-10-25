#pragma once
#include "LineupAtmosphere.h"
namespace attendance::situational {
enum class Event {None,FailedConversion,DeepIncomplete,DeepDefensiveIncomplete,DeepVisitorCatch,HomeInterception,HomeTackleForLoss,HomeTurnover,MissedHomeFieldGoal,MissedVisitorFieldGoal};
inline const char* name(Event e){switch(e){case Event::FailedConversion:return "Home failed conversion";case Event::DeepIncomplete:return "Home deep incompletion";case Event::DeepDefensiveIncomplete:return "Deep defensive incompletion / breakup";case Event::DeepVisitorCatch:return "Visitor deep completion";case Event::MissedHomeFieldGoal:return "Home missed field goal";case Event::MissedVisitorFieldGoal:return "Opponent missed field goal";case Event::HomeTurnover:return "Home turnover";case Event::HomeInterception:return "Home pass intercepted";case Event::HomeTackleForLoss:return "Home defensive tackle for loss";default:return "inactive";}}
struct Outcome {uint64_t id=0;int type=95;Event event=Event::None;};
class Model {
    uintptr_t match_=0;unsigned epoch_=0;uint64_t sequence_=0,last_=0,serial_=0;
    bool flight_=false,caught_=false,sent_=false,ready_=false,fieldGoal_=false;int phase_=-1,readyType_=-1;float air_=0;
public:
    void reset(){*this=Model{};}
    Outcome observe(const energy::Play& p,const energy::Play& snap,const energy::Outcome& result,
                    uintptr_t match,unsigned epoch,uint64_t sequence,uint64_t now,bool allowed,
                    int kind,int ball,float catchSpot,float ballSpot,bool held,int selectedType=3){
        if(match!=match_||epoch!=epoch_){reset();match_=match;epoch_=epoch;}
        if(!allowed||!match||p.simulated||!valid(p.game)||now<last_||(last_&&now-last_>750)){
            sequence_=0;last_=now;flight_=caught_=sent_=ready_=false;phase_=-1;air_=0;return {};
        }
        last_=now;
        bool snapSeen=p.game.phase==3&&phase_==1&&ready_&&energy::normalDown(snap);
        if(p.game.phase==1){ready_=energy::normalDown(p);readyType_=selectedType;}else if(p.game.phase==3)ready_=false;
        phase_=p.game.phase;
        if(!sequence)return {};
        if(sequence_!=sequence){
            if(!snapSeen)return {};
            sequence_=sequence;flight_=caught_=sent_=false;air_=0;fieldGoal_=readyType_==21;
        }
        auto emit=[&](int type,Event event){sent_=true;return Outcome{++serial_,type,event};};
        bool live=p.game.phase==3,post=p.game.phase==4||p.game.phase==5||p.game.phase==1||p.game.phase==7;
        if(fieldGoal_){
            if(!post||sent_||!result.accepted||!result.completed||result.sequence!=sequence||result.homePoints||result.awayPoints||
               std::abs(p.game.homeScore-p.game.awayScore)>8||p.offense==snap.offense)return {};
            return snap.offense==0&&!p.game.neutral?emit(95,Event::MissedHomeFieldGoal):emit(59,Event::MissedVisitorFieldGoal);
        }
        bool axis=std::isfinite(snap.fieldSpot)&&std::fabs(snap.fieldSpot)<=50&&(snap.attackDirection==1||snap.attackDirection==-1);
        if(live&&kind==3&&ball==1&&!held){
            flight_=true;
            if(axis&&std::isfinite(ballSpot)&&std::fabs(ballSpot)<=60)air_=(std::max)(air_,(ballSpot-snap.fieldSpot)*snap.attackDirection);
        }
        if(live&&flight_&&kind==1&&!caught_&&axis&&std::isfinite(catchSpot)&&std::fabs(catchSpot)<=50){
            caught_=true;float yards=std::round((catchSpot-snap.fieldSpot)*snap.attackDirection);
            if(!sent_&&!p.game.neutral&&snap.offense==1&&p.offense==1&&yards>=15&&yards<=100)return emit(95,Event::DeepVisitorCatch);
        }
        if(live&&flight_&&kind==2&&!sent_&&!p.game.neutral&&snap.offense==0&&p.offense==1)return emit(95,Event::HomeInterception);
        if(!post||sent_||!result.accepted||!result.completed||result.sequence!=sequence||result.homePoints||result.awayPoints)return {};
        bool success=result.firstDown||result.takeaway;
        if(!p.game.neutral&&snap.offense==0){
            if(result.takeaway)return emit(95,p.interception?Event::HomeInterception:Event::HomeTurnover);
            if(snap.down>=3&&!success&&(p.offense!=snap.offense||p.down>snap.down))return emit(95,Event::FailedConversion);
            if(flight_&&!caught_&&air_>=15&&!p.interception&&!p.fumble)return emit(95,Event::DeepIncomplete);
        }else{
            // At neutral sites, use the positive defensive family for either
            // defense. Do not paint the whole split crowd as disappointed by
            // the other half's interception or successful deep completion.
            if(flight_&&!caught_&&air_>=15&&!result.takeaway&&!p.interception&&!p.fumble)return emit(59,Event::DeepDefensiveIncomplete);
            float gain=energy::gainedYards(snap,p);
            if(p.offense==snap.offense&&gain<=-.5f&&gain>=-100)return emit(59,Event::HomeTackleForLoss);
        }
        return {};
    }
};
}
