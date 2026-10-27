#include "../Source/MotionSoundPairing.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    for(auto kind:{motion::Kind::Ordinary,motion::Kind::Critical,motion::Kind::LivePlay,motion::Kind::Touchdown,motion::Kind::Eruption,motion::Kind::Safety,motion::Kind::BlockedKick,motion::Kind::Clincher}){
        for(float fullness:{.5f,.8f,1.f}){
            motion::Cue cue{123,1,1400,1000,6000,kind,10,false,15};
            sustained::Intent out;out.active=true;out.match=123;out.epoch=1;out.updated=1400;
            auto paired=pairing::merge(out,cue,1400,fullness,1,true);
            assert(paired.intensity>0&&(paired.continuous||paired.reaction)&&paired.reason!=sustained::Reason::None);
            assert(!paired.liveCheer&&!paired.lineupCheer&&!paired.resultCheer); // A swell suffices; no invented cheer.
            out.reason=sustained::Reason::Lineup;out.continuous=true;out.intensity=0;
            paired=pairing::merge(out,cue,1400,fullness,1,true);assert(paired.intensity>0&&(paired.continuous||paired.reaction));
            out.reason=sustained::Reason::Eruption;out.celebration=8;
            paired=pairing::merge(out,cue,1400,fullness,1,true);assert(paired.intensity>0&&paired.celebration==8&&paired.reason==sustained::Reason::Eruption);
            pairing::SoundLease sound{123,1,1400,true};assert(pairing::soundReady(sound,cue,1400));
            sound.audible=false;assert(!pairing::soundReady(sound,cue,1400));sound.audible=true;
            assert(!pairing::soundReady(sound,cue,1551));assert(!pairing::soundReady(sound,cue,1399));
            ++sound.epoch;assert(!pairing::soundReady(sound,cue,1400));--sound.epoch;++sound.match;assert(!pairing::soundReady(sound,cue,1400));
        }
    }
    std::puts("PASS: every crowd motion category has a bounded sound companion, silent-intent repair, no fabricated cheers and audible lease fail-closed/stale/world/pause isolation.");
}
