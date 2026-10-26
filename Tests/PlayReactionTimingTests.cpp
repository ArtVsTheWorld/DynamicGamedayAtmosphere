#include "../Source/PlayReactionTiming.h"
#include "../Source/ReturnAtmosphere.h"
#include "../Source/DirectorShake.h"
#include "../Source/CoreAtmosphere.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    for(float cutoff:{0.f,10.f,15.f,20.f,30.f,60.f})for(unsigned cap:{0u,1000u,4000u,8000u}){
        unsigned old=0;for(int n=0;n<=11000;++n){float yards=n*.01f;auto extra=timing::extraMs(yards,cutoff,cap);assert(extra<=cap&&extra>=old);if(yards<=cutoff)assert(extra==0);if(yards>=99)assert(extra==cap);old=extra;}
    }
    assert(timing::extraMs(57,15)==2000&&timing::extraMs(99,15)==4000&&timing::extraMs(NAN,15)==0&&timing::extraMs(111,15)==0&&timing::extraMs(-1,15)==0);
    live::Settings setting;timing::Distance distance;energy::Play p;p.game={2,14,14,3,300,720};p.down=1;p.offense=0;
    auto tick=[&](uint64_t now,float yards,int phase=3,live::Outcome event={}){p.game.phase=phase;distance.observe(p,123,1,1,now,true,event,yards,yards,false,1,true,{1000,1,yards},-1,setting,4000);};
    for(int yards=10;yards<=99;++yards)tick(1000+yards*100,static_cast<float>(yards));
    assert(distance.ownsDuration()&&distance.extension()==4000);tick(11000,99,4);tick(11200,99,5);tick(11500,99,1);assert(distance.extension()==4000); // Selection does not add more time.
    live::ReturnAtmosphere sound;live::Outcome run{live::Event::LongRun,1,0,99};p.game.phase=3;
    sound.observe(p,123,1,20000,true,run,true,false,4500,{},4000,true);
    sustained::Intent base;base.active=true;base.match=123;base.epoch=1;base.releaseMs=14000;auto live=sound.merge(base,20000,1);assert(live.continuous&&live.releaseMs==7300);
    motion::Model camera;motion::Settings cameraSettings;motion::Input x;x.match=123;x.epoch=1;x.allowed=true;x.phase=3;x.now=20000;x.duringPlay=1;x.distanceExtraMs=4000;camera.observe(cameraSettings,x);
    x.now=20250;camera.observe(cameraSettings,x);p.game.phase=4;x.phase=4;x.now=20500;auto cue=camera.observe(cameraSettings,x);assert(cue.tail&&cue.until==29000);
    sound.observe(p,123,1,20500,true,{},false,false,4500,{},4000,true);auto tail=sound.merge(base,20500,1);assert(tail.continuous&&!tail.reaction&&tail.supporting&&tail.releaseMs==7300);
    sustained::Episode episode;episode.started(live,20000);tail.updated=20500;assert(!episode.mustStop(tail,20500)); // The native voice holds briefly before its single release.
    for(uint64_t now=20750;now<29000;now+=250){p.game.phase=now<22000?5:1;if(p.game.phase==1)p.offense=1;x.phase=p.game.phase;x.now=now;cue=camera.observe(cameraSettings,x);sound.observe(p,123,1,now,true,{},false,false,4500,{},4000,true);tail=sound.merge(base,now,1);assert(cue.until==29000&&tail.reaction==(now>=21700)&&tail.intensity>0);if(now==21750){tail.updated=now;assert(episode.mustStop(tail,now));episode.stopped(now,tail.releaseMs);assert(episode.quietUntil==29050);}
    } // Native polling may add one step, never a second fade.
    x.now=29000;assert(camera.observe(cameraSettings,x).kind==motion::Kind::None);sound.observe(p,123,1,29000,true,{},false,false,4500,{},4000,true);assert(sound.merge(base,29000,1).reason==sustained::Reason::None);
    // Touchdown ownership wins over the return and extends only its release.
    core::Settings c;core::Model crowd;energy::Outcome touchdown;touchdown.accepted=touchdown.completed=true;touchdown.sequence=1;touchdown.event=energy::Event::HomeScore;touchdown.homePoints=6;touchdown.preSnapMargin=0;
    p.game.phase=4;crowd.react(c,touchdown,30,p.game,true,true,.5f,4000,true);assert(crowd.resultCheer&&crowd.resultScoring);
    auto normal=postplay::choose(touchdown,p.game);assert(crowd.resultProfile.hold==normal.hold&&crowd.resultProfile.release==normal.release+4);
    crowd.update(c,p,30,1,1);auto scored=crowd.intent(c,p,123,1,30000,true);assert(scored.scoringReaction);assert(sound.merge(scored,29000,1).scoringReaction);
    motion::Model tdCamera;x={};x.match=123;x.epoch=1;x.allowed=true;x.phase=4;x.now=30000;x.touchdown=1;x.distanceExtraMs=4000;cue=tdCamera.observe(cameraSettings,x);assert(cue.until==45000&&cue.distanceExtraMs==4000);
    x.now=31000;assert(motion::strength(tdCamera.observe(cameraSettings,x),x.now)==20); // The opening hit did not stretch.
    // Interceptions/recoveries measure return distance from the actual pickup.
    for(auto family:{live::Event::Interception,live::Event::FumbleRecovery}){
        timing::Distance returns;p.game.phase=3;p.offense=1;returns.observe(p,123,1,1,1000,true,{},0,0,false,1,true,{},-1,setting,4000);
        p.offense=0;returns.observe(p,123,1,1,1100,true,{family,1,0,0},40,0,false,0,true,{},-1,setting,4000);
        for(int n=1;n<=99;++n)returns.observe(p,123,1,1,1100+n*100,true,{},40.f-n,0,false,0,true,{},-1,setting,4000);
        assert(returns.extension()==4000);p.game.phase=4;returns.observe(p,123,1,1,11100,true,{},-59,0,false,0,false,{},-1,setting,4000);assert(returns.extension()==4000);
        p.game.phase=3;returns.observe(p,123,1,2,11300,true,{},0,0,false,1,true,{},-1,setting,4000);assert(!returns.ownsDuration()&&!returns.extension());
    }
    // A final offensive TD can supply its official distance, but short scores,
    // safeties and unrelated result sequences cannot inherit a long tail.
    timing::Distance scoring;p.game.phase=3;p.offense=0;
    scoring.observe(p,123,1,1,12000,true,{},0,0,false,1,true,{},-1,setting,4000);
    energy::Play snap=p;touchdown.side=0;touchdown.touchdownYards=99;
    assert(scoring.forResult(touchdown,snap,4000)==4000);
    touchdown.touchdownYards=2;assert(scoring.forResult(touchdown,snap,4000)==0);
    touchdown.touchdownYards=99;touchdown.safety=true;assert(scoring.forResult(touchdown,snap,4000)==0);touchdown.safety=false;
    touchdown.sequence=2;assert(scoring.forResult(touchdown,snap,4000)==0);touchdown.sequence=1;
    // Kick returns have their existing 30-yard activation point. Unknown
    // return direction earns no invented distance; teleports earn no credit.
    timing::Distance kicks,unknown;
    kicks.observe(p,123,1,1,12000,true,{live::Event::KickReturn,1,0,30},0,0,false,1,true,{},30,setting,4000);assert(kicks.extension()==0);
    kicks.observe(p,123,1,1,12250,true,{},0,0,false,1,true,{},99,setting,4000);assert(kicks.extension()==4000);
    unknown.observe(p,123,1,1,12000,true,{live::Event::Interception,1,0,0},40,0,false,0,true,{},-1,setting,4000);
    unknown.observe(p,123,1,1,12250,true,{},39,0,false,0,true,{},-1,setting,4000);assert(unknown.extension()==0);
    p.offense=1;unknown.observe(p,123,1,2,12500,true,{},0,0,false,1,true,{},-1,setting,4000);
    p.offense=0;unknown.observe(p,123,1,2,12750,true,{live::Event::FumbleRecovery,2,0,0},40,0,false,0,true,{},-1,setting,4000);
    unknown.observe(p,123,1,2,13000,true,{},-59,0,false,0,true,{},-1,setting,4000);assert(unknown.extension()==0);
    p.game.phase=3;p.offense=0;distance.observe(p,123,1,2,12000,false,{},99,99,false,1,true,{},-1,setting,4000);assert(!distance.extension());
    live::ReturnAtmosphere bounded;p.game.phase=3;p.offense=0;
    for(uint64_t now=50000;now<=100000;now+=250){bounded.observe(p,123,1,now,true,now==50000?run:live::Outcome{},true,false,4500,{50000,1,99},4000,true);if(now>=95000)assert(!bounded.merge(base,now,1).continuous);}
    std::puts("PASS: smooth bounded distance tails, exact threshold/99-yard endpoints, zero/custom settings, single synchronized native/camera fade, fixed onset, no per-frame accumulation, scoring priority and return-origin/new-snap cleanup.");
}
