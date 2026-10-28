#pragma once
#include "LiveReactions.h"
#include "FumbleContact.h"
namespace attendance::live {
inline bool isFumbleState(int ball,int substate){return ball==5&&(substate==1||substate==2);}
inline void noteForcedResult(energy::Outcome& result,uint64_t sequence,const Outcome& forced){
    if(forced.id&&forced.event==Event::ForcedFumble&&result.accepted&&result.completed&&result.sequence==sequence&&!result.homePoints&&!result.awayPoints){
        result.forcedFumble=true;result.side=forced.side;result.event=energy::preferred(result.event,energy::Event::ForcedFumble);
    }
}
struct ForcedEvent {uintptr_t object=0;int type=-1;bool valid=false;};
class Fumbles {
    uintptr_t match_=0,event_=0;unsigned epoch_=0;uint64_t last_=0,began_=0,serial_=0,ended_=0;
    int phase_=-1,lastHeld_=-1,type_=-1,fumbler_=-1;
    bool ready_=false,live_=false,armed_=false,used_=false,pending_=false,forced_=false,sawLoose_=false,recoveryUsed_=false;
public:
    void reset(){*this=Fumbles{};}
    bool homeBallLost()const{return live_&&pending_&&fumbler_==0;}
    RunBuildup recoveryBuildup(uint64_t)const{return {};}
    Outcome observe(const energy::Play& p,uintptr_t match,unsigned epoch,uint64_t now,bool allowed,const ForcedEvent& event,int kind,int ball,bool held,bool fumbleState=false,fumblecontact::Proof contact={}){
        if(match!=match_||epoch!=epoch_){reset();match_=match;epoch_=epoch;}
        if(!allowed||!match||!valid(p.game)||p.simulated||now<last_||(last_&&now-last_>750)){ready_=live_=false;phase_=-1;last_=now;return {};}
        last_=now;Outcome out;
        if(p.game.phase==1){ready_=energy::normalDown(p)||(p.down==0&&(p.offense==0||p.offense==1));event_=event.object;type_=event.type;lastHeld_=-1;live_=false;}
        if(p.game.phase==3&&phase_==1&&ready_){live_=true;began_=now;ended_=0;armed_=type_!=32;used_=pending_=forced_=sawLoose_=recoveryUsed_=false;fumbler_=-1;}
        if(live_&&now>=began_&&now-began_>45000){live_=false;}
        if(live_&&phase_==3&&(p.game.phase==4||p.game.phase==5))ended_=now;
        bool post=(p.game.phase==4||p.game.phase==5)&&ended_&&now>=ended_&&now-ended_<=750;
        if(live_&&!used_&&contact.id&&contact.owner>=0&&contact.owner<=1&&((p.game.phase==3&&!ended_)||post)){
            used_=pending_=forced_=true;fumbler_=contact.owner;recoveryUsed_=false;
            if(fumbler_==1||p.game.neutral)out={Event::ForcedFumble,++serial_,1-fumbler_,0};
        }
        if(live_&&p.game.phase==3&&!ended_&&now>=began_&&now-began_<=45000){
            if(event.valid&&event.type!=32)armed_=true;
            int owner=lastHeld_;
            if(owner<0&&held&&kind==1&&(p.offense==0||p.offense==1))owner=p.offense;
            if(!used_&&event.valid&&event.type==32&&(armed_||event.object!=event_)&&owner>=0){
                used_=pending_=forced_=true;fumbler_=owner;recoveryUsed_=false;
                if(owner==1||p.game.neutral)out={Event::ForcedFumble,++serial_,1-owner,0};
            }
            if(fumbleState&&!held&&ball==5&&lastHeld_>=0){pending_=sawLoose_=true;fumbler_=lastHeld_;}
            if(pending_&&!held)sawLoose_=true;
            if(kind==2&&!forced_&&!sawLoose_){pending_=false;recoveryUsed_=true;} // An interception alone is not fumble proof.
            bool groundedHeld=held&&(ball==2||ball==3||ball==9);
            if(pending_&&!recoveryUsed_&&groundedHeld&&p.offense==1-fumbler_&&(forced_||sawLoose_)){
                recoveryUsed_=true;
                if(p.offense==0||p.game.neutral){out={Event::FumbleRecovery,++serial_,p.offense,0};}
            }else if(pending_&&sawLoose_&&held&&p.offense==fumbler_){pending_=false;recoveryUsed_=true;}
            if(held&&(groundedHeld||kind==1)&&(p.offense==0||p.offense==1))lastHeld_=p.offense;
        }
        if(p.game.phase!=1&&p.game.phase!=3&&!post)ready_=live_=false;
        phase_=p.game.phase;return out;
    }
};
}
