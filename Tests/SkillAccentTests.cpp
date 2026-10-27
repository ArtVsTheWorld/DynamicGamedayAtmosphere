#include "../Source/SkillSwell.h"
#include "../Source/MotionSoundPairing.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    assert(live::cheerType({live::Event::Truck,1,0,0})==11);
    for(auto event:{live::Event::StiffArm,live::Event::Hurdle,live::Event::Juke,live::Event::Spin,live::Event::BrokenTackle})assert(live::cheerType({event,1,0,0})==0);
    assert(live::cheerType({live::Event::HitStick,1,0,0})==59);
    motion::Settings settings;motion::Model camera;motion::Input in;in.match=123;in.epoch=1;in.phase=3;in.now=1000;
    in.allowed=in.duringAllowed=in.duringImpact=in.softAccent=true;in.duringPlay=1;in.impactMilliseconds=2000;in.reactionScale=.5f;
    auto cue=camera.observe(settings,in);assert(cue.softAccent&&cue.impactOnly&&motion::strength(cue,1000)==0);
    in.duringPlay=0;float previous=0;
    for(uint64_t now:{1100ull,1200ull,1300ull,1450ull}){in.now=now;cue=camera.observe(settings,in);float level=motion::strength(cue,now);assert(level>=previous&&level<=5.001f);previous=level;}
    assert(std::fabs(previous-5)<.001f);in.now=2000;cue=camera.observe(settings,in);assert(motion::strength(cue,2000)>0&&motion::strength(cue,2000)<5);
    in.now=2999;cue=camera.observe(settings,in);assert(motion::strength(cue,in.now)<.01f);in.now=3000;assert(camera.observe(settings,in).kind==motion::Kind::None);
    // Sound owns its own short lifetime and does not depend on camera settings.
    skillswell::Model swell;energy::Play p;p.game={2,14,14,1,300,720};p.down=1;p.offense=0;
    p.game.phase=3;swell.observe(p,123,1,1000,true,{live::Event::Hurdle,1,0,0},1);
    sustained::Intent base;base.active=true;base.match=123;base.epoch=1;
    for(uint64_t now:{1000ull,1200ull,1450ull,1800ull,2400ull,2999ull}){
        swell.observe(p,123,1,now,true,{},0);base.updated=now;auto out=swell.merge(base,now,1);
        assert(out.supportingOnly&&out.supporting&&out.attackMs==500&&out.releaseMs==1550&&out.size==2);
        assert(!out.liveCheer&&!out.resultCheer&&!out.celebration&&out.intensity<=650);
        if(now==1450)assert(out.intensity==650);
        auto score=base;score.scoringReaction=true;score.reason=sustained::Reason::Result;score.priority=5;
        assert(!swell.merge(score,now,1).supporting);
        score=base;score.celebration=1;assert(!swell.merge(score,now,1).supporting);
    }
    swell.observe(p,123,1,3000,true,{},0);base.updated=3000;assert(!swell.merge(base,3000,1).supporting);
    swell.observe(p,123,1,3100,true,{live::Event::Spin,2,0,0},2);swell.observe(p,123,1,3200,false,{},0);base.updated=3200;assert(!swell.merge(base,3200,1).supporting);
    std::puts("PASS: gentle 450ms skill build/fade, bounded 50% live strength, independent short swell without cheers/gain, native truck big-hit family, hit-stick separation and scoring/suspension priority.");
}
