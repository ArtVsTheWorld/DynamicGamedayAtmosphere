#pragma once
#include <array>
#include <cstdint>
#include <algorithm>
namespace attendance::majorreplay {
enum class Kind {None,Touchdown,Takeaway};
enum class Stage {Idle,WaitingFull,Full,WaitingInset,InsetRequested,Inset,Done,Unavailable};
enum class Notice {None,Armed,FullStarted,FullFinished,InsetRequested,InsetStarted,Finished,Unavailable};
struct Input {uintptr_t match=0;unsigned epoch=0;uint64_t now=0;int phase=0;bool allowed=false,paused=false,replaying=false,dispatch=true;};
// Native node addresses are identity keys only. They are read solely inside
// their borrowed native update callbacks, never retained for later access.
class Model {
    uintptr_t match_=0;unsigned epoch_=0;uint64_t sequence_=0,began_=0,last_=0,pausedAt_=0,finishedAt_=0;
    Kind kind_=Kind::None;Stage stage_=Stage::Idle;std::array<uintptr_t,16> active_{};
    unsigned completed_=0;bool queued_=false,sent_=false,fullNotice_=false;Notice notice_=Notice::None;
public:
    void reset(){*this=Model{};}
    bool owns(uintptr_t match,unsigned epoch)const{return match&&match==match_&&epoch==epoch_;}
    Kind kind()const{return kind_;}Stage stage()const{return stage_;}uint64_t sequence()const{return sequence_;}
    Notice takeNotice(){auto n=notice_;notice_=Notice::None;return n;}
    void arm(uintptr_t match,unsigned epoch,uint64_t now,uint64_t sequence,Kind kind){
        if(!match||!sequence||kind==Kind::None||(owns(match,epoch)&&sequence_==sequence))return;
        reset();match_=match;epoch_=epoch;sequence_=sequence;began_=last_=now;kind_=kind;stage_=Stage::WaitingFull;notice_=Notice::Armed;
    }
    bool routing(uintptr_t match,unsigned epoch)const{return owns(match,epoch)&&stage_==Stage::WaitingFull&&!sent_;}
    bool suppressInset(uintptr_t match,unsigned epoch){
        if(!owns(match,epoch)||stage_==Stage::Idle||stage_==Stage::Unavailable)return false;
        if(sent_)return true; // A later authored Event12 cannot restart PIP.
        if(stage_==Stage::WaitingFull||stage_==Stage::Full||stage_==Stage::WaitingInset){queued_=true;return true;}
        return false;
    }
    void node(uintptr_t match,unsigned epoch,uintptr_t identity,bool complete){
        if(!owns(match,epoch)||!identity||sent_||stage_==Stage::Unavailable||stage_==Stage::Idle)return;
        auto found=std::find(active_.begin(),active_.end(),identity);
        if(complete){if(found!=active_.end()){*found=0;++completed_;}return;}
        if(found!=active_.end())return;
        auto free=std::find(active_.begin(),active_.end(),uintptr_t{});
        if(free==active_.end()){stage_=Stage::Unavailable;notice_=Notice::Unavailable;return;}
        *free=identity;
    }
    bool tick(const Input& x){
        if(!owns(x.match,x.epoch)||!x.allowed||x.now<last_||x.phase==0||x.phase==6||x.phase==7||(!x.replaying&&x.phase==3)){reset();return false;}
        last_=x.now;
        if(x.paused){if(!pausedAt_)pausedAt_=x.now;return false;}
        if(pausedAt_){auto gap=x.now-pausedAt_;began_+=gap;if(finishedAt_)finishedAt_+=gap;pausedAt_=0;}
        if(stage_==Stage::Idle||stage_==Stage::Done||stage_==Stage::Unavailable)return false;
        if(x.replaying&&x.phase==4){stage_=Stage::Full;if(!fullNotice_){notice_=Notice::FullStarted;fullNotice_=true;}finishedAt_=0;}
        if(x.replaying&&x.phase==5&&sent_){if(stage_!=Stage::Inset)notice_=Notice::InsetStarted;stage_=Stage::Inset;}
        else if(!x.replaying&&stage_==Stage::Inset){stage_=Stage::Done;notice_=Notice::Finished;}
        bool active=std::any_of(active_.begin(),active_.end(),[](uintptr_t v){return v!=0;});
        // Native interception/fumble packages contain two serial full-screen
        // clips. Waiting for their completion prevents a gap between angles
        // from being mistaken for the end of the full-screen sequence.
        unsigned required=kind_==Kind::Takeaway?2u:1u;
        if(fullNotice_&&!x.replaying&&!active&&completed_>=required&&!sent_&&(x.phase==4||x.phase==5)){
            if(!finishedAt_){finishedAt_=x.now;stage_=Stage::WaitingInset;notice_=Notice::FullFinished;}
            if(x.dispatch&&x.now-finishedAt_>=250){sent_=true;stage_=Stage::InsetRequested;notice_=Notice::InsetRequested;return true;}
        }
        // Never invent a recording or force playback of unavailable assets.
        // Give an intercepted native PIP request back if its full route failed.
        if(x.dispatch&&!sent_&&x.now-began_>60000){stage_=Stage::Unavailable;notice_=Notice::Unavailable;if(queued_&&(x.phase==4||x.phase==5)){sent_=true;return true;}}
        return false;
    }
    void dispatchFailed(){stage_=Stage::Unavailable;notice_=Notice::Unavailable;sent_=false;}
};
}
