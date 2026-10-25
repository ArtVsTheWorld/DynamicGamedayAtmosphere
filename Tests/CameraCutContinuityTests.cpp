#include "../Source/DirectorShake.h"
#include "../Source/ExcitementRules.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    motion::Settings s;
    // Post-play cuts and the next lineup retain a TD's original clock.
    motion::Model camera;motion::Input x;x.match=123;x.epoch=1;x.allowed=true;x.phase=4;x.now=1000;x.touchdown=1;
    auto cue=camera.observe(s,x);auto until=cue.until,anchor=cue.oscillationBegan;
    for(uint64_t now=1250;now<11000;now+=250){x.now=now;x.phase=now<2000?4:now<3000?2:now<5000?5:1;x.lineup=x.venueEligible=x.phase==1;x.lineupId=2;
        cue=camera.observe(s,x);assert(cue.kind==motion::Kind::Touchdown&&cue.until==until&&cue.oscillationBegan==anchor&&motion::strength(cue,now)>0);}
    x.now=11000;x.phase=3;x.lineup=false;cue=camera.observe(s,x);assert(cue.kind!=motion::Kind::Touchdown&&cue.kind!=motion::Kind::LivePlay); // A new snap ends the old celebration; its own lineup can settle normally.
    // A live return settles through cuts; the next lineup cannot reset/extend it.
    camera.reset();x={};x.match=123;x.epoch=1;x.allowed=true;x.phase=3;x.now=20000;x.duringPlay=1;
    camera.observe(s,x);x.now=20250;camera.observe(s,x);x.now=20500;x.phase=4;cue=camera.observe(s,x);until=cue.until;anchor=cue.oscillationBegan;
    float previous=motion::strength(cue,x.now);
    for(uint64_t now=20750;now<until;now+=250){x.now=now;x.phase=now<21500?5:1;cue=camera.observe(s,x);float level=motion::strength(cue,now);assert(cue.kind==motion::Kind::LivePlay&&cue.until==until&&cue.oscillationBegan==anchor&&level<=previous);previous=level;}
    x.now=until;assert(camera.observe(s,x).kind==motion::Kind::None);
    // An authoritative sack arriving only at the next lineup is admitted once.
    x.now=30000;x.phase=1;x.duringPlay=2;x.duringImpact=true;cue=camera.observe(s,x);assert(cue.kind==motion::Kind::LivePlay&&cue.impactOnly);
    until=cue.until;x.now=30250;assert(motion::strength(camera.observe(s,x),x.now)>8);x.now=30500;assert(camera.observe(s,x).until==until);
    // Pause/interactive replay uses suspension; do not replay the consumed hit.
    x.allowed=false;x.now=30750;assert(camera.observe(s,x).kind==motion::Kind::None);x.allowed=true;x.now=31000;assert(camera.observe(s,x).kind==motion::Kind::None);
    // A confirmed result on phase 1 really can be a defensive sack.
    energy::Tracker plays;energy::Settings rules;energy::Play p;p.game={1,0,0,1,720,720};p.offense=1;p.down=1;p.toFirst=10;p.toGoal=60;p.fieldSpot=-10;p.attackDirection=1;p.resultStamp=1;
    plays.observe(rules,p);p.game.phase=3;plays.observe(rules,p);p.game.seconds=715;p.game.phase=1;p.down=2;p.toFirst=15;p.toGoal=65;p.fieldSpot=-15;p.resultStamp=2;p.sack=true;
    auto result=plays.observe(rules,p);assert(result.completed&&result.sack&&result.side==0);
    std::puts("PASS: TD/live impact continuity through cuts, timeout transitions and selection, unchanged expiry/oscillation, fresh-snap and pause/replay cleanup, and once-only late-confirmed defensive sack.");
}
