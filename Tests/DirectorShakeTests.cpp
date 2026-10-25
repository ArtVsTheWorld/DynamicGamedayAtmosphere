#include "../Source/DirectorShake.h"
#include <cassert>
#include <iostream>
#include <limits>
using namespace attendance::motion;
int main(){
    Settings s;assert(s.ordinary==5&&s.critical==10&&s.touchdownMs==11000&&s.lineupCheer==5);
    assert(lineupStrength(s,false,0)==3&&lineupStrength(s,false,1)==5);
    assert(lineupStrength(s,true,0)==5&&lineupStrength(s,true,1)==10);
    {auto custom=s;custom.ordinary=custom.critical=0;assert(lineupStrength(custom,false,1)==0&&lineupStrength(custom,true,0)==0);
        custom.ordinary=1;custom.critical=3;assert(lineupStrength(custom,false,0)==1&&lineupStrength(custom,true,0)==3);}
    s.ordinary=5;s.critical=10;s.touchdownMs=9000; // Explicit legacy profile exercises the existing envelope math.
    Input x;x.match=123;x.epoch=1;x.now=1000;x.phase=1;x.allowed=x.lineup=true;x.lineupId=1;
    Model m;auto c=m.observe(s,x);assert(c.kind==Kind::None); // A critical down alone cannot qualify.
    x.critical=true;x.now=1100;c=m.observe(s,x);assert(c.kind==Kind::None);
    x.venueEligible=true;x.critical=false;x.now=2000;c=m.observe(s,x);assert(strength(c,2000)==0);
    x.now=3000;c=m.observe(s,x);assert(strength(c,3000)==5);
    x.critical=true;c=m.observe(s,x);assert(strength(c,3000)==10);
    x.lineup=false;x.phase=3;x.now=3100;c=m.observe(s,x);assert(c.tail&&strength(c,3100)==10);
    x.now=3550;c=m.observe(s,x);assert(std::fabs(strength(c,3550)-5)<.001);
    x.now=4000;c=m.observe(s,x);assert(c.kind==Kind::None);
    x.phase=4;x.now=4100;x.touchdown=1;c=m.observe(s,x);assert(c.kind==Kind::Touchdown&&c.until==13100);
    x.now=4300;c=m.observe(s,x);assert(strength(c,4300)==20);
    x.now=5100;c=m.observe(s,x);assert(std::fabs(strength(c,5100)-20)<.001);
    x.now=5600;c=m.observe(s,x);assert(strength(c,5600)==15);
    x.now=9000;c=m.observe(s,x);assert(strength(c,9000)==15);
    x.now=11100;c=m.observe(s,x);assert(std::fabs(strength(c,11100)-7.5)<.001);
    x.now=13100;c=m.observe(s,x);assert(c.kind==Kind::None);
    x.now=13200;x.touchdown=2;c=m.observe(s,x);
    x.now=13500;x.eruption=1;x.eruptionAlive=true;c=m.observe(s,x);assert(c.kind==Kind::Eruption&&c.began==13200&&strength(c,13500)==20);
    x.now=15200;c=m.observe(s,x);assert(strength(c,15200)==22.5f);
    x.now=21200;c=m.observe(s,x);assert(strength(c,21200)==22.5f);
    x.now=25200;c=m.observe(s,x);assert(std::fabs(strength(c,25200)-11.25f)<.001);
    assert(strength(c,26000)==0); // Stale producer cancels even a long eruption.
    x.now=29200;c=m.observe(s,x);assert(c.kind==Kind::None); // Same IDs cannot retrigger.
    x.now=30000;x.touchdown=3;c=m.observe(s,x);assert(c.kind==Kind::Touchdown);
    x.now=30200;x.phase=1;x.lineup=true;x.lineupId=2;x.critical=false;c=m.observe(s,x);
    assert(c.kind==Kind::Touchdown&&strength(c,30200)>0); // A camera cut into lineup retains the earned celebration.
    x.now=31200;c=m.observe(s,x);assert(c.kind==Kind::Touchdown&&strength(c,31200)>=15&&strength(c,31200)<=20);
    x.allowed=false;x.now=31300;c=m.observe(s,x);assert(c.kind==Kind::None); // Disabling motion cancels immediately.
    x.allowed=false;x.now=31400;x.touchdown=4;c=m.observe(s,x);assert(c.kind==Kind::None);
    x.allowed=true;x.phase=4;x.lineup=false;x.now=31500;c=m.observe(s,x);assert(c.kind==Kind::None); // No replay of blocked event.
    x.epoch=2;x.now=32000;x.touchdown=0;x.eruption=0;x.lineup=true;x.lineupId=1;x.phase=1;
    c=m.observe(s,x);assert(c.epoch==2&&c.began==32000);
    s.ordinary=0;x.now=33000;c=m.observe(s,x);assert(strength(c,33000)==0);
    x.now=31000;c=m.observe(s,x);assert(c.kind==Kind::None); // Time rollback cancels.
    s.ordinary=999;s.critical=-1;s.touchdown=std::numeric_limits<float>::quiet_NaN();
    assert(configured(s,Kind::Ordinary)==100&&configured(s,Kind::Critical)==0&&configured(s,Kind::Touchdown)==0);
    // During-play motion has priority over the lineup tail and settles after the whistle.
    Settings liveSettings;liveSettings.reactionFadeMs=2000;liveSettings.impactExtraMs=0;liveSettings.ordinary=5;Model liveModel;Input v;v.match=999;v.epoch=1;v.allowed=true;v.venueEligible=true;v.phase=3;v.now=50000;v.duringPlay=1;
    auto liveCue=liveModel.observe(liveSettings,v);assert(liveCue.kind==Kind::LivePlay);
    v.now=50200;liveCue=liveModel.observe(liveSettings,v);assert(strength(liveCue,v.now)==10);
    auto anchor=liveCue.oscillationBegan;v.now=50900;liveModel.observe(liveSettings,v);v.phase=4;v.now=51000;liveCue=liveModel.observe(liveSettings,v);assert(liveCue.tail&&strength(liveCue,v.now)==10&&liveCue.oscillationBegan==anchor);
    v.now=52000;liveCue=liveModel.observe(liveSettings,v);assert(std::fabs(strength(liveCue,v.now)-5)<.001);
    v.now=53000;liveCue=liveModel.observe(liveSettings,v);assert(liveCue.kind==Kind::None);
    v.phase=3;v.now=54000;v.duringPlay=2;liveModel.observe(liveSettings,v);v.phase=4;v.now=54200;v.touchdown=1;liveCue=liveModel.observe(liveSettings,v);assert(liveCue.kind==Kind::Touchdown);
    v.allowed=false;v.now=54300;assert(liveModel.observe(liveSettings,v).kind==Kind::None);
    Model immediateModel;Input hit;hit.match=555;hit.epoch=1;hit.allowed=hit.duringAllowed=true;hit.phase=4;hit.now=60000;hit.duringPlay=1;
    auto impact=immediateModel.observe(liveSettings,hit);assert(impact.kind==Kind::LivePlay&&impact.impactOnly);
    hit.now=60200;impact=immediateModel.observe(liveSettings,hit);assert(strength(impact,hit.now)>9);
    hit.now=61400;impact=immediateModel.observe(liveSettings,hit);assert(strength(impact,hit.now)>0&&strength(impact,hit.now)<6);
    hit.now=62400;assert(immediateModel.observe(liveSettings,hit).kind==Kind::None);
    // Loss of competitive context fades an existing live cue rather than cutting it.
    hit.phase=3;hit.now=63000;hit.duringPlay=2;immediateModel.observe(liveSettings,hit);hit.now=63300;immediateModel.observe(liveSettings,hit);
    hit.duringAllowed=false;hit.now=63400;impact=immediateModel.observe(liveSettings,hit);assert(impact.tail&&strength(impact,hit.now)>0);
    hit.now=64400;impact=immediateModel.observe(liveSettings,hit);assert(strength(impact,hit.now)>0&&strength(impact,hit.now)<6);
    // Upgrades preserve both current amplitude and the oscillation clock.
    Model joined;Input j;j.match=777;j.epoch=1;j.allowed=true;j.phase=3;j.now=70000;j.duringPlay=1;
    joined.observe(liveSettings,j);j.now=70200;auto moving=joined.observe(liveSettings,j);assert(strength(moving,j.now)==10);
    j.phase=4;j.now=70400;j.touchdown=1;auto scoredCue=joined.observe(liveSettings,j);assert(scoredCue.kind==Kind::Touchdown&&strength(scoredCue,j.now)==10&&scoredCue.oscillationBegan==moving.oscillationBegan);
    j.now=70600;scoredCue=joined.observe(liveSettings,j);assert(strength(scoredCue,j.now)==20);
    j.eruption=1;j.eruptionAlive=true;auto eruptedCue=joined.observe(liveSettings,j);assert(eruptedCue.kind==Kind::Eruption&&strength(eruptedCue,j.now)==20&&eruptedCue.oscillationBegan==moving.oscillationBegan);
    j.now=70800;eruptedCue=joined.observe(liveSettings,j);assert(strength(eruptedCue,j.now)==30);
    Model kick;Input k;k.match=888;k.epoch=1;k.allowed=k.lineup=k.venueEligible=k.openingKickoff=true;k.lineupId=1;k.phase=1;k.now=80000;
    kick.observe(liveSettings,k);k.now=81000;assert(strength(kick.observe(liveSettings,k),k.now)==5);
    k.lineup=false;k.phase=3;k.now=82000;auto heldKick=kick.observe(liveSettings,k);assert(!heldKick.tail&&strength(heldKick,k.now)==5);
    k.now=82500;kick.observe(liveSettings,k);k.phase=4;k.now=83000;heldKick=kick.observe(liveSettings,k);assert(heldKick.tail&&heldKick.until==85000&&strength(heldKick,k.now)==5);
    k.now=84000;assert(std::fabs(strength(kick.observe(liveSettings,k),k.now)-2.5f)<.001f);
    k.now=85000;assert(kick.observe(liveSettings,k).kind==Kind::None);
    std::array<float,16> projection{};projection[0]=2;projection[5]=3.1715948f;projection[11]=1;
    assert(projectionScale(projection,Kind::Eruption)==1);projection[5]=12;
    assert(projectionScale(projection,Kind::Eruption)<.27f&&projectionScale(projection,Kind::Touchdown)<.27f);
    assert(projectionScale(projection,Kind::Ordinary)==1&&projectionScale(projection,Kind::LivePlay)==1);
    projection[5]=500;assert(projectionScale(projection,Kind::Eruption)<.007f);
    projection[5]=NAN;assert(projectionScale(projection,Kind::Eruption)==.35f);
    std::array<float,16> source{1,0,0,0,0,1,0,0,0,0,1,0,12,-30,90,1};
    c={123,1,1000,0,5000,Kind::Eruption,30,false};auto output=source;
    assert(apply(output,c,1000)&&output!=source&&validMatrix(output));
    // A pure camera-space rotation preserves camera position and zoom.
    for(int col=0;col<3;++col){float before=0,after=0;for(int row=0;row<3;++row){before-=source[12+row]*source[col*4+row];after-=output[12+row]*output[col*4+row];}assert(std::fabs(before-after)<.001);}
    assert(output[3]==0&&output[7]==0&&output[11]==0&&output[15]==1);
    auto invalid=source;invalid[0]=2;auto saved=invalid;assert(!apply(invalid,c,1000)&&invalid==saved);
    invalid=source;invalid[1]=std::numeric_limits<float>::infinity();assert(!validMatrix(invalid));
    c.percent=0;output=source;assert(!apply(output,c,1000)&&output==source);
    std::cout<<"PASS: configurable motion, venue gating, impact-hold-fade profiles, earned post-TD carry until expiry/new snap, event priority/deduplication, pause/new-world cleanup and camera-position-preserving matrix rotation.\n";
}
