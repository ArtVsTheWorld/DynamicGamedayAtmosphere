#pragma once
#include <cstdint>
// Native offsets for the inspected CFB27 layout. Signature checks are required;
// compatibility is not tied to an executable hash. Keep future binding changes
// here, separately from the configurable atmosphere rules.
namespace attendance::bindings {
inline constexpr uintptr_t AudioUpdate=0x28484a0,CrowdQueue=0x3d3fb30;
inline constexpr uintptr_t AttendanceUpdate=0x3632a50;
// GameStates post-update dispatches every active phase, including PreGame.
inline constexpr uintptr_t PresentationUpdate=0x5d0d530;
inline constexpr uintptr_t MixerCreate=0x10eb500,MixerRelease=0x10ed380,MixerManager=0xdc462f0;
inline constexpr uintptr_t MatchWrapper=0xf8576b0,ClockManager=0xf8572b8,GameStates=0xf8575a0;
inline constexpr uintptr_t CurrentDownGetter=0x61fa610,CurrentOffenseGetter=0x61fba90;
inline constexpr uintptr_t TimeoutGetter=0x5d7b540,FirstDownGetter=0x61fb3a0;
inline constexpr uintptr_t FirstDownLine=0x5d4eda0,TurnoverGetter=0x2e91820;
inline constexpr uintptr_t PlayStats=0xf3d1fe0,SelectedPlayer=0xd9ce380,PlayerContexts=0xe3405a0;
inline constexpr uintptr_t MatchVtable=0xb4781e8,RulesVtable=0xb4776f0;
// Native IsNeutralStadium, also used by HFA suppression at 0x2e1f5b4.
inline constexpr uintptr_t NeutralGetter=0x61faea0;
// Crowd slider in MixerBridge's cached user-volume array.
inline constexpr uintptr_t CrowdSlider=0x1f4;
// Scoped crowd-event query and owned temporary event lifecycle.
inline constexpr uintptr_t SwellQuery=0x2076c70,SwellQueryGet=0x207ca70,SwellQueryDestroy=0x20778a0,SwellWeakRelease=0x1498660;
inline constexpr uintptr_t SwellAllocator=0xdc4c670,SwellQueryType=0xf5ba3a8,SwellManagerGetter=0x3d3d500;
inline constexpr uintptr_t SwellLookup=0x3d54e20,SwellConstruct=0x3d4d190,SwellDestroy=0x3d4e180,SwellSetInt=0x3d50cd0,SwellSend=0x3d5b330;
inline constexpr uintptr_t SwellArenaInit=0x113a630,SwellArenaDestroy=0x113a9f0,SwellArenaVtable=0xad8fc98;
// Output-only native excitement; never writes the native envelope or HFA.
inline constexpr uintptr_t IntensityUpdate=0x28414c0,IntensityVtable=0xb221680;
inline constexpr uintptr_t IntensityLock=0x114dba0,IntensityUnlock=0x114ea00,IntensityPublish=0x5b2d020;
inline constexpr uintptr_t ReactionUpdate=0x2841720,ReactionReceive=0x2840d70,ReactionVtable=0xb221830;
// IsPaused UI accessor reads this same flag; only read it after verification.
inline constexpr uintptr_t PauseCheck=0x7ba6d50,PauseFlag=0xf547a00;
// Replay manager: verified leaf getters expose playback and recording times.
// These fields are read only; live game clocks do not rewind with highlights.
inline constexpr uintptr_t ReplayManager=0xe36dbf8,ReplayVtable=0xc7553f8;
inline constexpr uintptr_t ReplayTimeGetter=0xa1444e0,RecordingTimeGetter=0xa1444f0;
inline constexpr uintptr_t ReplayActiveGetter=0xa1445d0,ReplayStartGetter=0xa144610;
inline constexpr uintptr_t ReplayUpdate=0x288bcd0,ReplayEntityVtable=0xb22ffe0;
// CFB post-play expression evaluation. Match complete authored selectors;
// expression objects are discovered by evaluation, never by scanning the heap.
inline constexpr uintptr_t MuseLogicEvaluate=0x9134550,MuseComparisonValue=0x91320c0;
inline constexpr uintptr_t MuseLogicVtable=0xbf1bcc8,MuseComparisonVtable=0xbf1bc40;
inline constexpr uintptr_t MuseConstantVtable=0xb6462d0,MuseFunctionVtable=0xbc7c140,MuseActorFunctionVtable=0xbc7c270;
inline constexpr uintptr_t GameStatesVtable=0xb479070;
}
