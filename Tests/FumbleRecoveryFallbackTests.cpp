#include "../Source/LiveReactions.h"
#include "../Source/LiveFumble.h"
#include "../Source/CoreAtmosphere.h"
#include "../Source/DirectorShake.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    energy::Settings rules;
    for(int phase:{1,4,5})for(int offense:{0,1})for(bool neutral:{false,true}){
        energy::Tracker tracker;energy::Play p;p.game={2,7,7,1,300,720};p.game.neutral=neutral;
        p.down=1;p.offense=offense;p.toGoal=60;p.toFirst=10;p.resultStamp=10;
        tracker.observe(rules,p);p.game.phase=3;tracker.observe(rules,p);
        p.game.phase=phase;p.offense=1-offense;p.down=1;p.resultStamp=11;p.fumble=true;p.game.seconds=295;
        auto result=tracker.observe(rules,p);assert(result.accepted&&result.completed&&result.takeaway&&!result.forcedFumble);
        assert(!live::completedImpact(result,p.game,true,0,83).id);
        assert(!live::cameraReaction(live::Event::FumbleRecovery));
        if(offense==1||neutral){
            core::Model sound;core::Settings settings;sound.react(settings,result,1,p.game,true);
            assert(result.event==energy::Event::FumbleRecovery&&sound.resultCheer&&sound.resultType==59);
            auto forced=result;forced.forcedFumble=true;
            auto impact=live::completedImpact(forced,p.game,true,0,83);
            assert(impact.event==live::Event::ForcedFumble&&live::cameraReaction(impact.event));
            assert(!live::completedImpact(forced,p.game,true,forced.sequence,59).id);
        }
    }
    // Own and defensive pickups never add a recovery buildup. The forced hit
    // remains independently eligible, and a subsequent pickup cannot reset it.
    for(int owner:{0,1}){
        live::Fumbles f;energy::Play p;p.game={2,7,7,1,300,720};p.offense=owner;p.down=1;live::ForcedEvent e{999,0,true};
        f.observe(p,123,1,1000,true,e,0,9,true);p.game.phase=3;f.observe(p,123,1,1100,true,e,0,9,true);
        e.type=32;auto hit=f.observe(p,123,1,1200,true,e,0,5,false,true);
        assert(bool(hit.id)==(owner==1));if(hit.id)assert(hit.event==live::Event::ForcedFumble);
        p.offense=owner;auto own=f.observe(p,123,1,1300,true,e,0,2,true);assert(!own.id&&!f.recoveryBuildup(1300).id);
    }
    // A held pickup must not revive a long-run ramp from pre-fumble yardage.
    live::Model run;live::Settings limits;energy::Play p;p.game={2,7,7,1,300,720};p.down=1;p.offense=0;p.fieldSpot=-30;p.attackDirection=1;
    uint64_t now=1000;auto sample=[&](int phase,float yards,bool held){now+=100;p.game.phase=phase;return run.observe(limits,p,123,1,now,true,456,0,held?2:5,NAN,-30+yards,held);};
    sample(1,0,true);sample(3,0,true);for(int n=1;n<=14;++n)sample(3,static_cast<float>(n),true);assert(run.runBuildup().id);
    run.cancelRunBuildup(true);p.fumble=true;sample(3,14,false);p.fumble=false;
    for(int n=15;n<=35;++n){assert(!sample(3,static_cast<float>(n),true).id);assert(!run.runBuildup().id);}
    sample(4,35,true);sample(1,0,true);sample(3,0,true);live::Outcome fresh;
    for(int n=1;n<=20;++n){auto event=sample(3,static_cast<float>(n),true);if(event.id)fresh=event;}
    assert(fresh.event==live::Event::LongRun);
    std::puts("PASS: recovery-only motion and buildup removed; defensive recovery audio retained; forced-fumble impact/dedup preserved; own pickups cannot revive prior gain motion; next snap rearms normally.");
}
