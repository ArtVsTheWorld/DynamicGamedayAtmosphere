#pragma once
#include "DirectorShake.h"

namespace attendance::cannon {
// Authored FX_STADIUM_EffectSwitch_PFB inputs Cannon_01..Cannon_09.
// Horns, bells, fireworks and gun effects are separate entries, not cannons.
inline bool variant(int value){return value>=10&&value<=18;}
inline constexpr unsigned PlayEvent=0x82b90c1c,EffectParameter=0xd173dd08;
inline constexpr const char* Patch="Sound/CFB/CoreAudio/SoundPatches/Stadium/SoundEffects/FX_STADIUM_SPECIFIC_FX_SP";
struct Blast {uintptr_t match=0;unsigned epoch=0;uint64_t began=0;int effect=-1;};
inline motion::Cue merge(const motion::Cue& crowd,const Blast& blast,uint64_t now,float configured,bool allowed){
    if(!allowed||!blast.match||!variant(blast.effect)||!blast.began||now<blast.began||now-blast.began>=900)return crowd;
    motion::Cue impact{blast.match,blast.epoch,now,blast.began,blast.began+900,motion::Kind::Cannon,motion::limit(configured),false};
    impact.impactOnly=true;impact.oscillationBegan=blast.began;
    bool same=crowd.match==blast.match&&crowd.epoch==blast.epoch;
    if(same&&motion::strength(crowd,now)>=motion::strength(impact,now))return crowd;
    // Share the crowd's oscillator when present. A blast never restarts or
    // adds to the underlying touchdown/eruption envelope; the louder wins.
    if(same&&crowd.kind!=motion::Kind::None)impact.oscillationBegan=crowd.oscillationBegan?crowd.oscillationBegan:crowd.began;
    return impact;
}
}
