#pragma once
#include "LiveReactions.h"
namespace attendance::live {
inline bool isFumbleState(int ball,int substate){return ball==5&&(substate==1||substate==2);}
struct ForcedEvent {uintptr_t object=0;int type=-1;bool valid=false;};
class Fumbles {
    uintptr_t match_=0,event_=0;unsigned epoch_=0;uint64_t last_=0,began_=0,serial_=0;
    int phase_=-1,lastHeld_=-1,type_=-1,fumbler_=-1,recoverySide_=-1;
    bool ready_=false,live_=false,armed_=false,used_=false,pending_=false,forced_=false,sawLoose_=false,recoveryUsed_=false;
    uint64_t recoveredAt_=0;
public:
    void reset(){*this=Fumbles{};}
    RunBuildup recoveryBuildup(uint64_t now)const{
        return live_&&phase_==3&&recoveredAt_&&now>=recoveredAt_?RunBuildup{recoveredAt_,(std::clamp)(static_cast<float>(now-recoveredAt_)/1500.f,.0001f,1.f),0}:RunBuildup{};
    }
    Outcome observe(const energy::Play& p,uintptr_t match,unsigned epoch,uint64_t now,bool allowed,const ForcedEvent& event,int kind,int ball,bool held,bool fumbleState=false){
        if(match!=match_||epoch!=epoch_){reset();match_=match;epoch_=epoch;}
        if(!allowed||!match||!valid(p.game)||p.simulated||now<last_||(last_&&now-last_>750)){ready_=live_=false;phase_=-1;last_=now;return {};}
        last_=now;Outcome out;
        if(p.game.phase==1){ready_=energy::normalDown(p)||(p.down==0&&(p.offense==0||p.offense==1));event_=event.object;type_=event.type;lastHeld_=-1;live_=false;recoveredAt_=0;}
        if(p.game.phase==3&&phase_==1&&ready_){live_=true;began_=now;armed_=type_!=32;used_=pending_=forced_=sawLoose_=recoveryUsed_=false;fumbler_=recoverySide_=-1;}
        if(live_&&now>=began_&&now-began_>45000){live_=false;recoveredAt_=0;}
        if(live_&&p.game.phase==3&&now>=began_&&now-began_<=45000){
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
            if(pending_&&!recoveryUsed_&&held&&ball==9&&p.offense==1-fumbler_&&(forced_||sawLoose_)){
                recoveryUsed_=true;
                if(p.offense==0||p.game.neutral){recoveredAt_=now;recoverySide_=p.offense;out={Event::FumbleRecovery,++serial_,p.offense,0};}
            }else if(pending_&&sawLoose_&&held&&p.offense==fumbler_){pending_=false;recoveryUsed_=true;}
            if(recoveredAt_&&(!held||p.offense!=recoverySide_))recoveredAt_=0;
            if(held&&(ball==9||kind==1)&&(p.offense==0||p.offense==1))lastHeld_=p.offense;
        }
        if(p.game.phase!=1&&p.game.phase!=3)ready_=live_=false;
        phase_=p.game.phase;return out;
    }
};
}
