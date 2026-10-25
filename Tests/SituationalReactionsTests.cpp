#include "../Source/SituationalReactions.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
struct Case {
    situational::Model m;energy::Play p,snap;energy::Outcome result;uint64_t sequence=0,time=1000;bool allowed=true;
    Case(int side=0,bool neutral=false){p.game={2,7,7,1,400,720};p.game.neutral=neutral;p.offense=side;p.down=3;p.fieldSpot=-30;p.attackDirection=1;p.toFirst=10;snap=p;tick(1,0,2,0,true);sequence=1;tick(3,0,2,0,true);}
    situational::Outcome tick(int phase,int kind,int ball,float gain,bool held){p.game.phase=phase;time+=100;return m.observe(p,snap,result,123,1,sequence,time,allowed,kind,ball,-30+gain,-30+gain,held);}
    void finish(float gain,bool first=false){result.accepted=result.completed=true;result.sequence=sequence;result.yards=gain;result.firstDown=first;p.fieldSpot=snap.fieldSpot+gain;p.down=first?1:4;}
};
int main(){
    {Case c;c.tick(3,3,1,20,false);c.finish(0);auto o=c.tick(4,3,1,20,false);assert(o.type==95&&o.event==situational::Event::FailedConversion);assert(!c.tick(4,3,1,20,false).id);}
    {Case c;c.snap.down=c.p.down=1;c.tick(3,3,1,15,false);c.finish(0);assert(c.tick(4,3,1,15,false).event==situational::Event::DeepIncomplete);}
    {Case c(1);c.tick(3,3,1,15,false);c.finish(0);auto o=c.tick(4,3,1,15,false);assert(o.type==59&&o.event==situational::Event::DeepDefensiveIncomplete);}
    {Case c(1);c.tick(3,3,1,14.9f,false);c.finish(0);assert(!c.tick(4,3,1,14.9f,false).id);}
    {Case c(1);c.tick(3,3,1,15,false);auto o=c.tick(3,1,4,15,true);assert(o.type==95&&o.event==situational::Event::DeepVisitorCatch);assert(!c.tick(3,1,4,30,true).id);}
    {Case c;c.tick(3,3,1,20,false);c.p.offense=1;auto o=c.tick(3,2,4,20,true);assert(o.type==95&&o.event==situational::Event::HomeInterception);}
    {Case c(1);c.finish(-1);auto o=c.tick(4,0,9,-1,false);assert(o.type==59&&o.event==situational::Event::HomeTackleForLoss);}
    {Case c(1);c.finish(-.1f);assert(!c.tick(4,0,9,-.1f,false).id);}
    {Case c;c.finish(12,true);assert(!c.tick(4,1,4,12,true).id);}
    {Case c;c.finish(0);c.result.homePoints=3;assert(!c.tick(4,0,9,0,false).id);}
    for(int side:{0,1}){
        Case c(side,true);c.tick(3,3,1,20,false);assert(!c.tick(3,1,4,20,true).id); // Split crowd: no global letdown for a positive catch.
        Case d(side,true);d.tick(3,3,1,20,false);d.finish(0);auto o=d.tick(4,3,1,20,false);assert(o.type==59&&o.event==situational::Event::DeepDefensiveIncomplete);
    }
    {Case c;c.allowed=false;c.tick(3,3,1,20,false);c.allowed=true;c.finish(0);assert(!c.tick(4,3,1,20,false).id);}
    {Case c;c.tick(3,3,1,20,false);c.time+=1000;c.finish(0);assert(!c.tick(4,3,1,20,false).id);}
    {Case c;c.p.simulated=true;c.tick(3,3,1,20,false);c.finish(0);assert(!c.tick(4,3,1,20,false).id);}
    std::puts("PASS: home disappointment, 15-air-yard defensive incompletions, immediate away deep-catch/INT letdown, home TFL, neutral symmetry, successful/score rejection, dedupe, stale/sim/lost-context guards.");
}
