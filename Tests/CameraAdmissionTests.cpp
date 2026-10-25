#include "../Source/LineupAtmosphere.h"
#include "../Source/DirectorShake.h"
#include <cassert>
#include <iostream>
using namespace attendance;
energy::Play fixture(){energy::Play p;p.game={2,14,14,1,600,720};p.offense=0;p.down=1;p.toGoal=75;p.toFirst=10;return p;}
int main(){
    core::Settings c;
    for(uint64_t n:{6060ull,26101ull,56099ull,61888ull,120000ull})for(int side:{0,1}){
        float standard=packed::standardMinimum(n),special=packed::specialMinimum(n),full=packed::minimum(n);
        auto p=fixture();p.offense=side;
        auto check=[&](float f,int type=3,bool known=true){return lineup::cameraAdmission(c,p,f,known,n,type);};
        assert(!check(standard).allowed); // Ordinary unranked calls still need packed house.
        p.down=3;
        for(float yards:{0.f,7.f,8.f,9.f,10.f}){p.toFirst=yards;auto cue=check(standard);assert(cue.allowed&&cue.critical);assert(!check(standard-.0001f).allowed);}
        p.toFirst=10.1f;assert(!check(standard).allowed);auto longThird=check(full);assert(longThird.allowed&&!longThird.critical);
        for(float invalid:{-1.f,NAN,INFINITY}){p.toFirst=invalid;assert(!check(standard).allowed);}
        p.toFirst=10;
        for(int type:{21,22,105,106,154,155}){assert(!check(standard,type).allowed);assert(check(full,type).allowed&&!check(full,type).critical);}
        p.down=4;assert(check(standard).allowed&&check(standard).critical);
        for(int type:{-1,0,21,22,23,24,25,37,38,105,106,154,155})assert(!check(standard,type).allowed);
        p.down=3;p.game.awayScore=23;assert(!check(standard).allowed);assert(check(full).allowed&&!check(full).critical);p.game.awayScore=14;
        p.down=1;p.game.quarter=5;p.game.seconds=0;
        auto ot=check(special);assert(ot.allowed&&!ot.critical&&!lineup::cameraVenue(c,p.game,special,true,n));
        assert(!check(special-.0001f).allowed);
        p.down=3;p.toFirst=10;assert(check(special).critical);p.toFirst=11;assert(check(special).allowed&&!check(special).critical);
        p.down=4;assert(check(special,21).allowed&&!check(special,21).critical); // OT field goal remains gentle.
        p.down=1;p.game.quarter=4;p.game.seconds=90;p.toGoal=30;p.offense=0;
        auto late=check(special);assert(late.allowed&&!late.critical);assert(!check(special-.0001f).allowed);
        p.offense=1;assert(!check(special).allowed);p.game.neutral=true;assert(check(special).allowed);p.game.neutral=false;p.offense=0;
        p.down=3;p.toFirst=10;assert(check(special).critical);p.toFirst=11;assert(check(special).allowed&&!check(special).critical);
        p.down=1;p.game.seconds=181;assert(!check(special).allowed);
        p=fixture();p.game.rank0=6;p.game.rank1=11;assert(check(standard).allowed&&!check(standard).critical);
        p.game.phase=3;assert(!check(1).allowed);p.game.phase=7;assert(!check(1).allowed);p.game.phase=1;p.simulated=true;assert(!check(1).allowed);p.simulated=false;
        assert(!check(NAN).allowed&&!check(1.1f).allowed&&!check(1,3,false).allowed&&!check(1,-1).allowed);
        c.cameraShake=false;assert(!check(1).allowed);c.cameraShake=true;c.lineups=false;assert(!check(1).allowed);c.lineups=true;
        assert(!lineup::cameraAdmission(c,p,1,true,0,3).allowed);
    }
    // Exercise the admission decision through the actual motion envelope.
    auto p=fixture();p.game.quarter=5;p.game.seconds=0;motion::Settings settings;
    for(bool critical:{false,true}){
        p.down=critical?3:1;p.toFirst=10;auto admit=lineup::cameraAdmission(c,p,.50f,true,61888,3);assert(admit.allowed&&admit.critical==critical);
        motion::Model model;motion::Input x;x.match=123;x.epoch=1;x.phase=1;x.now=1000;x.allowed=x.lineup=true;x.lineupId=1;x.lineupScale=.5f;x.venueEligible=admit.allowed;x.critical=admit.critical;
        model.observe(settings,x);x.now=2000;auto cue=model.observe(settings,x);float level=critical?7.5f:4.0f;
        assert(std::fabs(motion::strength(cue,x.now)-level)<.001f);
        x.phase=3;x.lineup=false;x.now=2100;assert(model.observe(settings,x).tail);x.now=3000;assert(model.observe(settings,x).kind==motion::Kind::None);
    }
    // Actual big games receive an opening allowance; ordinary small venues do not.
    p=fixture();p.game.quarter=1;p.game.seconds=720;p.game.rank0=6;p.game.rank1=11;
    assert(lineup::cameraAdmission(c,p,.50f,true,6060,3).allowed);
    assert(lineup::cameraAdmission(c,p,.90f,true,6060,3).allowed);
    std::cout<<"PASS: independent third-and-10/fourth-down critical motion, stable 3-5/5-10 envelopes, gentle OT/late downs, special teams, side/neutral scope, strict size gates, pause-phase/sim/unknown rejection and 50% big-game opening allowance.\n";
}
