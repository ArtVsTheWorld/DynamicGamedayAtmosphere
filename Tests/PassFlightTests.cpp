#include "../Source/LiveReactions.h"
#include <cassert>
#include <iostream>
using namespace attendance;
int main(){
    live::PassFlight air;energy::Play p;p.game={2,14,14,1,600,720};p.down=2;p.offense=0;
    uint64_t now=1000;bool allowed=true;uintptr_t match=123,record=456;unsigned epoch=1;
    auto sample=[&](int phase,int kind,int ball,bool held){p.game.phase=phase;now+=16;air.observe(p,match,epoch,now,allowed,record,kind,ball,held);};
    auto begin=[&](){sample(1,1,4,true);sample(3,0,2,true);};
    begin();sample(3,3,1,false);now+=160;sample(3,3,1,false);assert(air.weight(now)==1);
    sustained::Intent out;out.active=true;out.match=match;out.epoch=epoch;out.updated=now;
    auto anticipation=air.merge(out,now,.8f);assert(anticipation.reason==sustained::Reason::PassFlight&&anticipation.continuous&&anticipation.intensity>700);
    assert(!anticipation.liveCheer&&!anticipation.shake&&!anticipation.celebration);
    out.reason=sustained::Reason::Lineup;out.intensity=1000;assert(air.merge(out,now,.8f).intensity==1000); // No extra voice or reduction.
    out.reason=sustained::Reason::Eruption;assert(air.merge(out,now,.8f).reason==sustained::Reason::Eruption);
    sample(3,1,4,true);assert(air.weight(now)==1);now+=600;sample(4,1,4,true);assert(air.weight(now)>0&&air.weight(now)<.6f);
    now+=650;sample(4,1,4,true);assert(air.weight(now)==0);
    begin();sample(3,3,4,true);assert(air.weight(now)==0); // Holder/ball 4 is not flight.
    begin();sample(3,3,1,false);now+=160;sample(3,3,1,false);allowed=false;sample(3,3,1,false);assert(air.weight(now)==0);allowed=true;
    sample(3,3,1,false);assert(air.weight(now)==0); // No mid-flight attachment.
    begin();sample(3,3,1,false);now+=900;sample(3,3,1,false);assert(air.weight(now)==0);
    begin();sample(3,3,1,false);epoch=2;sample(3,3,1,false);assert(air.weight(now)==0);
    begin();record=789;sample(3,3,1,false);assert(air.weight(now)==0);record=456;
    begin();p.simulated=true;sample(3,3,1,false);assert(air.weight(now)==0);p.simulated=false;
    begin();sample(3,3,1,false);for(int i=0;i<82;++i){now+=100;sample(3,3,1,false);}assert(air.weight(now)<1);
    for(int i=0;i<15;++i){now+=100;sample(3,3,1,false);}assert(air.weight(now)==0); // Eight-second maximum, no restart.
    std::cout<<"PASS: traced airborne-pass state, existing-voice reuse, catch/incompletion fade, once per snap, bounded lease, no guessed catches, pause/sim/world/stale guards.\n";
}
