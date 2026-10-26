#include "../Source/LiveReactions.h"
#include "../Source/DirectorShake.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    energy::Settings rules;motion::Settings settings;
    for(int phase:{1,4,5})for(int offense:{0,1})for(bool neutral:{false,true}){
        energy::Tracker tracker;energy::Play p;p.game={2,7,7,1,300,720};p.game.neutral=neutral;
        p.down=1;p.offense=offense;p.toGoal=60;p.toFirst=10;p.resultStamp=10;
        tracker.observe(rules,p);p.game.phase=3;tracker.observe(rules,p);
        // The loose ball and pickup both happened between live observations.
        p.game.phase=phase;p.offense=1-offense;p.down=1;p.resultStamp=11;p.fumble=true;p.game.seconds=295;
        auto result=tracker.observe(rules,p);bool expected=offense==1||neutral;
        assert(result.accepted&&result.completed&&result.takeaway&&!result.forcedFumble);
        auto impact=live::completedImpact(result,p.game,true,0,83);assert(bool(impact.id)==expected);
        if(expected){
            assert(result.event==energy::Event::FumbleRecovery&&impact.event==live::Event::FumbleRecovery&&live::cheerType(impact)==59);
            assert(live::completedImpact(result,p.game,true,0,59).id); // A previous sound-only defensive cheer owns no camera cue.
            assert(!live::completedImpact(result,p.game,false,0,83).id);
            assert(!live::completedImpact(result,p.game,true,result.sequence,59).id); // Existing forced/pickup response wins.
            assert(live::completedImpact(result,p.game,true,result.sequence,95).id); // An earlier letdown is not that defensive response.
            motion::Model camera;motion::Input x;x.match=123;x.epoch=1;x.allowed=x.duringAllowed=x.duringImpact=true;
            x.phase=phase;x.now=1000;x.duringPlay=impact.id;auto cue=camera.observe(settings,x);auto until=cue.until;
            assert(cue.kind==motion::Kind::LivePlay&&cue.impactOnly&&cue.percent==10&&until==7400);
            for(uint64_t now=1200;now<until;now+=100){x.now=now;x.phase=now<2000?2:now<3000?5:1;cue=camera.observe(settings,x);assert(cue.kind==motion::Kind::LivePlay&&cue.until==until&&motion::strength(cue,now)>0);}
            x.now=until;assert(camera.observe(settings,x).kind==motion::Kind::None);
            camera.reset();x.now=10000;x.phase=4;x.touchdown=1;cue=camera.observe(settings,x);assert(cue.kind==motion::Kind::Touchdown);
            x.now=10100;x.eruption=1;x.eruptionAlive=true;assert(camera.observe(settings,x).kind==motion::Kind::Eruption);
        }
        assert(!tracker.observe(rules,p).completed);
    }
    for(int scenario=0;scenario<6;++scenario){
        energy::Tracker tracker;energy::Play p;p.game={2,7,7,1,300,720};p.down=1;p.offense=1;p.resultStamp=10;
        tracker.observe(rules,p);p.game.phase=3;tracker.observe(rules,p);
        p.game.phase=4;p.offense=scenario==0?1:0;p.resultStamp=scenario==1?10:11;p.fumble=scenario!=2;p.down=1;
        if(scenario==3)p.simulated=true;if(scenario==4){p.fumble=false;p.interception=true;}if(scenario==5)p.game.phase=7;
        auto result=tracker.observe(rules,p);assert(!live::completedImpact(result,p.game,true,0,83).id);
    }
    std::puts("PASS: confirmed fast defensive recoveries get one eligible 10% post-whistle impact without invented live pickup/return; home/neutral ownership, gates, native duplicate suppression, timeout/selection fade, TD/eruption priority and stale/own-recovery/interception/simulation guards.");
}
