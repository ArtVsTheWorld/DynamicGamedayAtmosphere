#include "../Source/DefensiveMotion.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
struct Case {
    defensivemotion::Model model;energy::Play snap,p;energy::Outcome result;
    uintptr_t match=123;unsigned epoch=1;uint64_t sequence=1,now=1000,prior=0;bool allowed=true;
    Case(int down=1,int side=1,bool neutral=false){
        p.game={2,14,14,1,300,720};p.game.neutral=neutral;p.down=down;p.offense=side;
        p.selectedType=103;p.toFirst=10;p.toGoal=60;p.fieldSpot=-10;p.attackDirection=1;p.resultStamp=10;
        snap=p;tick(1);tick(3);
    }
    live::Outcome tick(int phase,live::Outcome evidence={}){
        now+=100;p.game.phase=phase;
        return model.observe(snap,p,result,evidence,match,epoch,sequence,now,allowed,prior);
    }
    void finish(float yards=0){
        result.accepted=result.completed=true;result.sequence=sequence;result.side=1-snap.offense;result.yards=yards;
        result.fiveYardLoss=yards<=-4.99f;p.down=snap.down+1;
        if(snap.down==4){p.offense=1-snap.offense;p.down=1;result.turnoverOnDowns=true;}
    }
};
int main(){
    // Measured loss, not native sack truth, determines standalone loss motion.
    unsigned cases=0;
    for(int down:{1,2,3,4})for(int side:{0,1})for(bool neutral:{false,true})
    for(int phase:{1,4,5})for(float yards:{-10.f,-5.f,-4.98f,-2.f,0.f})for(bool sack:{false,true}){
        Case c(down,side,neutral);c.finish(yards);c.result.sack=sack;
        auto evidence=live::completedImpact(c.result,c.p.game,true,0,59);
        auto out=c.tick(phase,evidence);bool loss=yards<=-5;bool expected=(side==1||neutral)&&(loss||down==4);
        assert(bool(out.id)==expected);
        if(out.id){assert(out.event==(loss?live::Event::TackleForLoss:live::Event::DownsStop));assert(out.side==1-side);}
        assert(!c.tick(phase,evidence).id);++cases;
    }
    for(float unknown:{NAN,-1000.f,-2.f}){
        Case c;c.finish(unknown);c.result.sack=true;c.p.game.phase=4;
        auto sound=live::completedImpact(c.result,c.p.game,true,0,59);assert(sound.event==live::Event::Sack);
        assert(!c.tick(4,sound).id&&defensivemotion::cameraId(sound,123,{})==0);
        assert(live::cheerType(sound)==59); // Sound stays distinct from motion.
    }
    // End-to-end tracker -> sound classifier -> shared camera router, including
    // independent line-of-scrimmage directions and a short native-confirmed sack.
    for(int direction:{-1,1})for(float yards:{-6.f,-5.f,-4.f})for(bool sack:{false,true}){
        Case c;c.p.attackDirection=direction;c.p.fieldSpot=-10.f*direction;c.snap=c.p;c.snap.game.phase=1;
        energy::Tracker tracker;energy::Settings settings;tracker.observe(settings,c.snap);
        c.p.game.phase=3;tracker.observe(settings,c.p);c.p.game.phase=4;c.p.down=2;
        c.p.game.seconds=295;c.p.resultStamp=11;c.p.fieldSpot+=yards*direction;c.p.toGoal-=yards;c.p.toFirst-=yards;c.p.sack=sack;
        c.result=tracker.observe(settings,c.p);assert(c.result.completed);
        auto sound=live::completedImpact(c.result,c.p.game,true,0,59);auto out=c.tick(4,sound);
        assert(bool(out.id)==(yards<=-5));
        auto id=defensivemotion::cameraId(sound,19,out);assert(id==out.id);
        motion::Model camera;motion::Settings cameraSettings;motion::Input x;
        x.match=123;x.epoch=1;x.now=c.now;x.phase=4;x.allowed=x.duringAllowed=true;x.duringPlay=id;x.duringImpact=out.id!=0;x.profile=motion::Profile::Loss;
        auto cue=camera.observe(cameraSettings,x);assert((cue.kind==motion::Kind::LivePlay)==(yards<=-5));
        if(out.id)assert(cue.impactOnly&&cue.percent==8&&cue.until==c.now+2700);
    }
    // Knockout/contact can arrive before the whistle, but cannot shake until
    // the shared completed stop is verified. Routine third-down incompletions
    // still need their existing impact flavor or high-leverage/goal condition.
    for(auto flavor:{live::Event::CatchKnockout,live::Event::PassBreakup,live::Event::HitStick,live::Event::HitStickStop}){
        for(int down:{1,2,3,4}){
            Case c(down);assert(!c.tick(3,{flavor,1,0,0}).id);assert(!c.tick(4).id);
            c.finish();auto out=c.tick(5);assert(bool(out.id)==(down>=3));
            if(out.id)assert(out.event==(flavor==live::Event::HitStick?live::Event::HitStickStop:flavor));
            assert(!c.tick(1,{flavor,2,0,0}).id);
        }
    }
    {Case c(3);c.finish();assert(!c.tick(4).id);assert(c.tick(5,{live::Event::PassBreakup,1,0,0}).event==live::Event::PassBreakup);}
    {Case c(3);c.finish();for(int n=0;n<52;++n)assert(!c.tick(4).id);assert(!c.tick(5,{live::Event::PassBreakup,1,0,0}).id);} // No lease refresh.
    {Case c(3);c.finish();assert(!c.tick(4,{live::Event::CatchKnockout,1,1,0}).id);} // Wrong defender.
    {Case c(3);c.finish();c.p.down=3;assert(!c.tick(4,{live::Event::CatchKnockout,1,0,0}).id);} // Stop not established.
    for(int kind=0;kind<4;++kind){Case c(kind==0?4:3);c.finish(kind==3?-7.f:0);
        c.result.goalLineStop=kind==0;c.result.thirdGoalStop=kind==1;c.result.highLeverageStop=kind==2;if(kind==2){c.snap.game.quarter=c.p.game.quarter=4;}
        auto out=c.tick(4,{live::Event::PassBreakup,1,0,0});
        assert(out.event==(kind==0?live::Event::GoalLineStop:kind==1?live::Event::PassBreakup:kind==2?live::Event::CriticalStop:live::Event::TackleForLoss));
    }
    for(int down:{3,4}){Case c(down);c.finish(-7);c.result.goalLineStop=down==4;c.result.thirdGoalStop=down==3;
        assert(c.tick(4).event==(down==4?live::Event::GoalLineStop:live::Event::TackleForLoss));} // Preserve goal-line profile over overlapping loss.
    // Ownership, accepted penalties, score correction, clock plays, gate loss,
    // replay/pause/gaps and resume cannot manufacture or queue defensive motion.
    for(int bad=0;bad<20;++bad){Case c(3);c.finish(-6);
        if(bad==0)c.allowed=false;if(bad==1)c.p.simulated=true;if(bad==2)c.p.resumed=true;
        if(bad==3)c.now+=800;if(bad==4)c.result.accepted=false;if(bad==5)c.result.completed=false;
        if(bad==6)++c.result.sequence;if(bad==7)c.snap.specialTeams=true;if(bad==8)c.snap.selectedType=37;
        if(bad==9)c.snap.selectedType=38;if(bad==10)c.result.firstDown=true;if(bad==11)c.result.takeaway=true;
        if(bad==12)c.p.fumble=true;if(bad==13)c.p.interception=true;if(bad==14)c.result.homePoints=6;
        if(bad==15)++c.p.game.homeScore;if(bad==16)c.result.forcedFumble=true;
        if(bad==17)c.result.blockedFieldGoal=true;if(bad==18)c.prior=c.sequence;if(bad==19)c.snap.down=-1;
        assert(!c.tick(4,{live::Event::CatchKnockout,1,0,0}).id);
    }
    for(auto bigger:{live::Event::ForcedFumble,live::Event::Interception,live::Event::BlockedFieldGoal}){
        Case c;c.finish(-7);assert(!c.tick(4,{bigger,1,0,0}).id);assert(!c.tick(5).id);
    }
    for(int interruption=0;interruption<3;++interruption){Case c(3);c.tick(3,{live::Event::CatchKnockout,1,0,0});
        if(interruption==0)c.model.suspend();if(interruption==1)c.allowed=false;
        assert(!c.tick(interruption==2?2:3).id);c.allowed=true;c.finish();assert(!c.tick(4).id);
        ++c.sequence;c.snap.game.phase=1;c.p=c.snap;c.result={};c.tick(1);c.tick(3);c.finish(-5);assert(c.tick(4).id);
    }
    {Case c;c.finish(-7);auto out=c.tick(4);assert(out.id);c.model.reset();++c.match;++c.epoch;c.result={};c.p=c.snap;c.tick(1);c.tick(3);c.finish(-7);assert(c.tick(4).id);} // New match context.
    for(auto event:{live::Event::Sack,live::Event::TackleForLoss,live::Event::GoalLineStop,live::Event::ThirdGoalStop,
                   live::Event::CriticalStop,live::Event::DownsStop,live::Event::CatchKnockout,live::Event::PassBreakup,live::Event::HitStickStop})
        assert(defensivemotion::cameraId({event,1,0,0},99,{})==0);
    for(auto event:{live::Event::LongRun,live::Event::Interception,live::Event::FumbleRecovery,live::Event::ForcedFumble,live::Event::LongCatch})
        assert(defensivemotion::cameraId({event,1,0,0},99,{})==99);
    for(auto event:{live::Event::Spin,live::Event::Juke,live::Event::BrokenTackle,live::Event::HitStick})
        assert(defensivemotion::cameraId({event,1,0,0},99,{})==0);
    assert(!defensivemotion::cameraId({},99,{}));
    std::printf("PASS: %u loss/sack/down/owner/phase combinations; tracker-to-camera integration; one confirmed stop; early/late contact flavor; exclusions, cleanup and audio independence.\n",cases);
}
