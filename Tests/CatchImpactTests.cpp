#include "../Source/DirectorShake.h"
#include <cassert>
#include <cstdio>
using namespace attendance::motion;
int main(){
    Settings settings;assert(settings.catchImpact==8);
    for(int phase:{4,5}){Model m;Input x;x.match=123;x.epoch=1;x.allowed=x.duringAllowed=x.duringImpact=x.duringCatch=true;x.phase=phase;x.now=1000;x.duringPlay=1;
        auto hit=m.observe(settings,x);assert(hit.impactOnly&&hit.catchReaction&&hit.percent==8&&hit.until==6900);
        x.now=1200;hit=m.observe(settings,x);assert(strength(hit,1200)>7.9f&&strength(hit,1200)<=8);
        x.now=6900;assert(m.observe(settings,x).kind==Kind::None);
    }
    for(unsigned delay:{16u,100u,350u,351u,1000u}){Model m;Input x;x.match=123;x.epoch=1;x.allowed=x.duringAllowed=x.duringCatch=true;x.phase=3;x.now=1000;x.duringPlay=1;
        m.observe(settings,x);x.duringPlay=0;
        for(unsigned update=100;update<delay;update+=100){x.now=1000+update;m.observe(settings,x);}
        x.phase=4;x.now=1000+delay;auto ended=m.observe(settings,x);
        assert(ended.impactOnly==(delay<=350));assert(ended.percent==(delay<=350?8:10));
        if(delay<=350){assert(ended.until==6900);x.now+=250;assert(strength(m.observe(settings,x),x.now)<=8.001f);}
    }
    {Model m;Input x;x.match=123;x.epoch=1;x.allowed=x.duringAllowed=x.duringImpact=true;x.phase=4;x.now=1000;x.duringPlay=1;
        assert(m.observe(settings,x).percent==10); // Sack/other impacts keep their own tested strength.
        x.touchdown=1;x.now=1100;assert(m.observe(settings,x).kind==Kind::Touchdown);
        x.eruption=1;x.eruptionAlive=true;x.now=1200;assert(m.observe(settings,x).kind==Kind::Eruption);}
    {Model m;Input x;x.match=123;x.epoch=1;x.phase=4;x.now=1000;x.duringPlay=1;x.duringCatch=true;assert(m.observe(settings,x).kind==Kind::None);}
    std::puts("PASS: separate 8% immediate-catch impact, bounded whistle classification, continuous caught-and-run strength, 5.9-second duration, sack separation, priority and disabled guards.");
}
