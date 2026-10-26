#include "../Source/PlayReactionTiming.h"
#include "../Source/ReturnAtmosphere.h"
#include "../Source/DirectorShake.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    for(float yards:{0.f,15.f,20.f,30.f,60.f,99.f,110.f,NAN})for(unsigned oldCap:{0u,4000u,8000u})assert(timing::extraMs(yards,15,oldCap)==0);
    for(auto event:{live::Event::LongRun,live::Event::RunAfterCatch,live::Event::Interception,live::Event::FumbleRecovery,live::Event::KickReturn})for(float yards:{15.f,40.f,99.f}){
        energy::Play p;p.game={2,14,14,3,300,720};p.down=1;p.offense=0;
        live::ReturnAtmosphere sound;motion::Model camera;motion::Settings settings;assert(settings.reactionFadeMs==5000);
        sustained::Intent base;base.active=true;base.match=123;base.epoch=1;
        motion::Input x;x.match=123;x.epoch=1;x.allowed=true;x.now=1000;x.phase=3;x.duringPlay=1;
        camera.observe(settings,x);sound.observe(p,123,1,1000,true,{event,1,0,yards},true,false,settings.reactionFadeMs);
        x.now=1250;camera.observe(settings,x);p.game.phase=4;x.phase=4;x.now=1500;x.duringPlay=0;
        auto cue=camera.observe(settings,x);sound.observe(p,123,1,1500,true,{},false,false,settings.reactionFadeMs);assert(cue.until==6500);
        for(uint64_t now=1750;now<6500;now+=250){x.now=now;p.game.phase=x.phase=now<3000?2:5;cue=camera.observe(settings,x);sound.observe(p,123,1,now,true,{},false,false,settings.reactionFadeMs);assert(cue.until==6500&&motion::strength(cue,now)>0&&sound.merge(base,now,1).reason==sustained::Reason::Result);}
        x.now=6500;assert(camera.observe(settings,x).kind==motion::Kind::None);sound.observe(p,123,1,6500,true,{},false,false,settings.reactionFadeMs);assert(sound.merge(base,6500,1).reason==sustained::Reason::None);
    }
    std::puts("PASS: fixed five-second live post-whistle settling at all play distances, no retired distance extension, synchronized return sound/motion, timeout/selection continuity and expiry.");
}
