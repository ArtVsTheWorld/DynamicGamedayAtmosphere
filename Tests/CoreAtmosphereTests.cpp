#include "../Source/CoreAtmosphere.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace attendance;
static bool near(float a,float b){return std::fabs(a-b)<0.001f;}
static energy::Play game(){energy::Play p;p.game={1,0,0,6,300,300};p.game.rivalry=true;p.offense=0;p.down=1;return p;}
static energy::Outcome result(energy::Event e,int side=0,uint64_t sequence=1){energy::Outcome o;o.accepted=true;o.event=e;o.side=side;o.sequence=sequence;return o;}
int main(){
    core::Settings c;
    // Captured 31-33 -> 34-33 winner: ball still in flight after phase 4,
    // then EndOfGame briefly reports phase 8 before returning to phase 7.
    {
        energy::Tracker tracker;energy::Settings settings;core::Model model;auto live=game();
        live.game={4,31,33,1,1.7338886f,300};live.offense=0;live.down=2;
        auto feed=[&](double time){
            normalizeFinalPresentation(live.game);auto observed=tracker.observe(settings,live);assert(observed.accepted);
            model.react(c,observed,time,live.game);model.update(c,live,time,1,1);
        };
        feed(0);live.game.phase=3;feed(1);live.game.phase=4;live.game.seconds=0;feed(3);
        live.game.homeScore=34;feed(4);assert(model.erupting);
        live.game.phase=5;feed(16);live.game.quarter=6;live.game.phase=8;feed(17);
        assert(model.postgameHeld&&model.intent(c,live,123,1,17000,true).reason==sustained::Reason::Eruption);
        live.game.phase=7;feed(17.1);feed(80);assert(model.postgameHeld&&model.parent==6&&model.celebration==1);
        live.game.phase=8;feed(81);live.game.phase=7;feed(82);assert(model.parent==6&&model.celebration==1);
        assert(model.finishPostgame());feed(83);assert(!model.erupting&&model.parent==0);
    }
    auto p=game();core::Model opening;opening.update(c,p,0,.5f,1);
    assert(opening.openingWeight==1&&near(opening.beds,4)&&near(opening.parent,2)&&opening.intensity==1000);
    auto intent=opening.intent(c,p,123,1,0,true);assert(intent.reason==sustained::Reason::Opening&&intent.continuous&&intent.size==3);
    for(int phase:{1,3,4,5,6}){p.game.phase=phase;auto i=opening.intent(c,p,123,1,100,true);assert(i.reason==sustained::Reason::Opening);}
    p.game.phase=3;p.game.seconds=295;opening.update(c,p,1,.5f,1);p.game.phase=1;p.offense=1;p.game.seconds=280;opening.update(c,p,2,.5f,1);
    assert(opening.openingProfile.fade>=0&&opening.intent(c,p,123,1,2000,true).reason==sustained::Reason::None);
    core::Model sparse;p=game();sparse.update(c,p,0,.5f,.5f);assert(sparse.beds<4&&sparse.intensity<1000);
    p=game();p.game.phase=1;p.game.seconds=200;core::Model late;late.update(c,p,0,1,1);assert(late.openingWeight==0);
    p.game.rivalry=false;core::Model ordinary;ordinary.update(c,p,0,1,1);assert(ordinary.parent==0&&ordinary.beds==0&&ordinary.intensity==0);
    for(int quarter:{2,3,4})for(int down:{1,3,4}){
        p=game();p.game={quarter,42,42,1,60,300};p.game.rank0=0;p.game.rank1=3;p.down=down;p.toGoal=5;
        core::Model m;m.update(c,p,0,1,1);assert(m.intensity==0&&m.intent(c,p,123,1,0,true).reason==sustained::Reason::None);
    }
    for(auto e:{energy::Event::PickSix,energy::Event::FourthDown,energy::Event::OpeningTouchdown,energy::Event::Turnover,energy::Event::ExplosiveGain,energy::Event::UpsetWin,energy::Event::LateDrive}){
        core::Model m;p=game();assert(!m.react(c,result(e),0,p.game));assert(m.eruptionAt<0);
    }
    for(float clock:{0.0f,60.0f,89.0f,90.0f}){
        core::Model m;p=game();p.game={4,23,20,4,clock,300};m.update(c,p,0,1,1);
        assert(m.react(c,result(energy::Event::LastSecondLead),0,p.game));float peak=0;
        for(int n=0;n<=1100;n++){
            m.update(c,p,n*.05,1,1);peak=(std::max)(peak,m.parent);assert(m.parent<=6&&m.limit<=6);
            float bed=mix::ambienceForTotal(m.parent,m.parent,m.beds,m.limit);assert(m.parent+bed<=m.limit+.001f);
            if(n>=900)assert(m.limit<=4&&m.intensity==0);
        }assert(peak>5.99f);
    }
    p=game();p.game={4,23,20,4,91,300};core::Model early;assert(!early.react(c,result(energy::Event::LastSecondLead),0,p.game));
    p.game.seconds=60;assert(!early.react(c,result(energy::Event::LastSecondLead,1),0,p.game));p.game.neutral=true;assert(early.react(c,result(energy::Event::LastSecondLead,1),0,p.game));
    assert(!early.react(c,result(energy::Event::LastSecondLead,1),2,p.game));
    double began=early.eruptionAt;p.game.phase=7;assert(!early.react(c,result(energy::Event::OvertimeWin,1),3,p.game)&&early.eruptionAt==began);
    p.game.neutral=false;auto visitor=result(energy::Event::None);visitor.visitorScore=true;
    assert(!early.react(c,visitor,4,p.game)&&early.eruptionAt<0);
    core::Model handoff;p=game();p.game={5,20,20,1,0,300};p.game.playoff=true;handoff.update(c,p,0,1,1);float before=handoff.parent;
    p.game.phase=7;p.game.homeScore=23;assert(handoff.react(c,result(energy::Event::OvertimeWin),1,p.game));handoff.update(c,p,1,1,1);
    assert(handoff.parent>=before);handoff.update(c,p,2,1,1);assert(handoff.parent>before);handoff.update(c,p,18,1,1);assert(handoff.parent>4);
    // The final score's roar stays full through even a long postgame presentation.
    for(double now:{47.0,180.0,600.0}){
        handoff.update(c,p,now,1,1);auto held=handoff.intent(c,p,123,1,static_cast<uint64_t>(now*1000),true);
        assert(handoff.postgameHeld&&handoff.erupting&&near(handoff.parent,6)&&near(handoff.intensity,1000));
        assert(held.reason==sustained::Reason::Eruption&&held.continuous&&held.celebration==1);
        assert(held.celebrationUntil<static_cast<uint64_t>(now*1000)); // No repeated cheers.
        assert(handoff.eruptionLease(static_cast<uint64_t>(now*1000))==static_cast<uint64_t>(now*1000)+750);
        assert(!handoff.intent(c,p,123,1,static_cast<uint64_t>(now*1000),false).active);
    }
    assert(handoff.finishPostgame()&&!handoff.finishPostgame());
    handoff.update(c,p,601,1,1);assert(!handoff.erupting&&handoff.parent==0&&handoff.intensity==0&&handoff.eruptionLease(601000)==0);
    assert(!handoff.intent(c,p,123,1,601000,true).active);
    assert(!handoff.react(c,result(energy::Event::GameWinningScore,0,2),602,p.game)); // Stats/highlights cannot restart it.
    // OT owns one continuous roar across snaps/possession labels, even in an ordinary game.
    core::Model ot;p=game();p.game.rivalry=false;p.game={5,20,20,1,0,720};
    ot.update(c,p,0,1,1);auto overtime=ot.intent(c,p,123,1,0,true);
    assert(overtime.reason==sustained::Reason::Overtime&&overtime.continuous&&ot.intensity==1000&&ot.parent<=4);
    for(int phase:{1,3,4,5}){p.game.phase=phase;assert(ot.intent(c,p,123,1,100,true).reason==sustained::Reason::Overtime);}
    // Natural opening release preserves excitement for twenty seconds.
    p=game();core::Model fade;fade.update(c,p,0,1,1);assert(fade.intent(c,p,123,1,0,true).releaseMs==20000);
    p.game.phase=3;p.game.seconds=295;fade.update(c,p,1,1,1);
    p.game.phase=5;p.offense=1;p.game.seconds=280;fade.update(c,p,2,1,1);
    assert(fade.intensity>990);fade.update(c,p,12,1,1);assert(fade.intensity>450&&fade.intensity<550);
    fade.update(c,p,23,1,1);assert(fade.intensity==0);
    // Eruptions request the tested roar and one cheer token, then release over ten seconds.
    core::Model layers;p=game();p.game={4,23,20,4,60,720};assert(layers.react(c,result(energy::Event::LastSecondLead),0,p.game));
    layers.update(c,p,1,1,1);auto cheer=layers.intent(c,p,123,1,1000,true);
    assert(cheer.reason==sustained::Reason::Eruption&&cheer.continuous&&cheer.celebration&&cheer.releaseMs==10000);
    layers.update(c,p,36,1,1);assert(layers.intent(c,p,123,1,36000,true).reason==sustained::Reason::None&&layers.parent>0);
    // A close major upset gets one final whistle celebration unless one is carrying.
    core::Model upset;p=game();p.game={4,28,24,5,10,720};p.game.rank0=127;p.game.rank1=3;
    upset.update(c,p,0,1,1);p.game.phase=7;p.game.quarter=6;p.game.seconds=0;upset.update(c,p,1,1,1);
    assert(upset.event==energy::Event::UpsetWin&&upset.celebration==1);
    upset.update(c,p,50,1,1);assert(upset.celebration==1);
    for(int kind:{0,1,2,3}){
        core::Model final;p=game();p.game={4,28,24,5,10,720};p.game.rank0=127;p.game.rank1=3;
        if(kind==0)p.game.homeScore=35;if(kind==1)p.game.rank0=26;
        if(kind==2){p.game.neutral=true;p.game=reversed(p.game);}
        if(kind!=3)final.update(c,p,0,1,1);
        p.game.phase=7;p.game.quarter=6;p.game.seconds=0;final.update(c,p,1,1,1);
        assert(final.celebration==(kind==2?1u:0u));if(kind==2)assert(final.winner==1);
    }
    core::Model carrying;p=game();p.game={4,28,24,5,10,720};p.game.rank0=127;p.game.rank1=3;
    carrying.update(c,p,0,1,1);carrying.react(c,result(energy::Event::LastSecondLead),1,p.game);carrying.update(c,p,2,1,1);
    p.game.phase=7;p.game.quarter=6;p.game.seconds=0;carrying.update(c,p,3,1,1);assert(carrying.celebration==1&&carrying.eruptionAt==1);
    for(bool neutral:{false,true}){
        core::Model lost;p=game();p.game={4,28,24,5,10,720};p.game.neutral=neutral;
        lost.react(c,result(energy::Event::LastSecondLead),0,p.game);lost.update(c,p,1,1,1);
        p.game.phase=7;p.game.awayScore=31;lost.update(c,p,2,1,1);assert(!lost.postgameHeld);
    }
    // Neutral-site away winners receive the same sustained postgame carry.
    core::Model neutralWin;p=game();p.game={4,24,28,5,10,720};p.game.neutral=true;
    neutralWin.react(c,result(energy::Event::LastSecondLead,1),0,p.game);neutralWin.update(c,p,1,.64f,.64f);
    p.game.phase=7;neutralWin.update(c,p,2,.64f,.64f);neutralWin.update(c,p,200,.64f,.64f);
    assert(neutralWin.postgameHeld&&near(neutralWin.parent,4.8f)&&neutralWin.intensity>799&&neutralWin.parent<=6);
    c.maximumPercent=100;c.eruptionPercent=100;assert(core::normalLimit(c)==4&&core::eruptionLimit(c)==6);
    assert(core::callbackLimit(c,100,100,101)==6&&core::callbackLimit(c,100,101,101)==4);
    c.eruptions=false;assert(core::callbackLimit(c,6,100,101)==4);c.enabled=false;assert(core::normalLimit(c)==0);
    c=core::Settings{};c.maximumPercent=0;core::Model disabled;p=game();assert(!disabled.react(c,result(energy::Event::GameWinningScore),0,p.game));disabled.update(c,p,0,1,1);assert(disabled.parent==0&&disabled.beds==0&&disabled.intensity==0);
    c.maximumPercent=25;c.eruptionPercent=50;assert(near(core::normalLimit(c),1.9382003f)&&near(core::eruptionLimit(c),3.521825f));
    assert(near(volume::boostPercent(core::normalLimit(c)),25)&&near(volume::boostPercent(core::eruptionLimit(c)),50));
    std::cout<<"PASS: three-feature audio, 4/6 dB caps, opening ownership, final-90-second/neutral winners, deduplication, postgame continuity and no incidental-play boosts.\n";
}
