#pragma once
#include <cstdint>
// Native offsets for the inspected CFB27 layout. Signature checks are required;
// compatibility is not tied to an executable hash. Keep future binding changes
// here, separately from the configurable atmosphere rules.
namespace attendance::bindings {
inline constexpr uintptr_t AudioUpdate=0x28484a0,CrowdQueue=0x3d3fb30;
inline constexpr uintptr_t AttendanceUpdate=0x3632a50;
inline constexpr uintptr_t CrowdVisibility=0x64afda0,PeriodChange=0x5d024c0,CrowdPartResolve=0x65047b0;
// Read-only placement lookup verified against the native agent*16 position path.
inline constexpr uintptr_t SceneCrowdVtable=0xb7db448,CrowdRendererVtable=0xb7db228,CrowdPositionKey=0xb222068;
// GameStates post-update dispatches every active phase, including PreGame.
inline constexpr uintptr_t PresentationUpdate=0x5d0d530;
inline constexpr uintptr_t MixerCreate=0x10eb500,MixerRelease=0x10ed380,MixerManager=0xdc462f0;
inline constexpr uintptr_t MatchWrapper=0xf8576b0,ClockManager=0xf8572b8,GameStates=0xf8575a0;
inline constexpr uintptr_t CurrentDownGetter=0x61fa610,CurrentOffenseGetter=0x61fba90;
inline constexpr uintptr_t TimeoutGetter=0x5d7b540,FirstDownGetter=0x61fb3a0;
inline constexpr uintptr_t FirstDownLine=0x5d4eda0,TurnoverGetter=0x2e91820;
inline constexpr uintptr_t FumblePredicate=0x61fc570;
inline constexpr uintptr_t SackPredicate=0x61fcc30,ForcedFumblePredicate=0x61fd360;
inline constexpr uintptr_t LiveFootballEvent=0xe34f548,LiveFootballEventVtable=0xc292c60,LiveFootballEventType=0x275bbe0;
inline constexpr uintptr_t PlayStats=0xf3d1fe0,SelectedPlayer=0xd9ce380,PlayerContexts=0xe3405a0;
// Same stadium DB capacity reader and attendance option used by
// FootballMatchConditionsConstData. Reads only; no attendance/HFA mutation.
inline constexpr uintptr_t StadiumCapacity=0x38bff60,OptionQuery=0x90934d0;
inline constexpr unsigned AttendanceOption=0x9e1d2500;
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
// Presentation camera interfaces and read-only accessibility settings.
inline constexpr uintptr_t CameraViewCopy=0x809c3e0,CameraViewSlot=0xbd0a7c0;
inline constexpr uintptr_t PlayCallService=0xe2cb918,PlayCallServiceVtable=0xb32c170,PlayCallTeamVtable=0xb2e2330,PlayCallSelectedVtable=0xb2e1f80,PlayCallVtable=0xb1dcf80;
inline constexpr uintptr_t OffensivePlayType=0x61fbb30,PlayCallTeamGet=0x2f23fd0,PlayCallSelectedGet=0x2b0f570,PlayCallGet=0x2b0f410,PlayCallType=0x27b4f80,PlayCallFallbackType=0x27b4e10;
inline constexpr uintptr_t BallControl=0xf8575f8,BallStateGetter=0x5d1fe50,CatchPosition=0x6208b90;
inline constexpr uintptr_t CameraOwner=0xfcd2a00,CameraGet=0xa17b3b0,CameraFind=0x809a3e0,CameraSet=0x80a6960,CameraVtable=0xbd0a6b0;
inline constexpr uintptr_t CameraEffectsGetter=0x61fdf50,CameraSettingGetter=0x5cf6180;
inline constexpr uintptr_t CameraSettings=0xe2c73f0,CameraSettingsVtable=0xb7d3180;
inline constexpr uintptr_t MatchVisualManager=0xe375d90,MatchVisualGetter=0x29f0a20,MatchVisualVtable=0xb265868;
inline constexpr uintptr_t HfaSelection=0xf3c7af8,HfaMeter=0xf3c7fd8,HfaMeterVtable=0xb2fa7e8,HfaDisabledGetter=0x2575ec0;
inline constexpr uintptr_t HfaSelectionVtable=0xb2fa030;
inline constexpr uintptr_t ReactionUpdate=0x2841720,ReactionReceive=0x2840d70,ReactionVtable=0xb221830;
// IsPaused UI accessor reads this same flag; only read it after verification.
inline constexpr uintptr_t PauseCheck=0x7ba6d50,PauseFlag=0xf547a00;
// Named pause-tile text output; separate from the shared UI string writer.
inline constexpr uintptr_t PauseMenuText=0x7a183cb,MenuStringWriter=0x25a3060,MenuTileVtable=0xb6465a0;
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
// Read only. UI IsInSuperSimGameplayMode checks this controller's mode +8.
inline constexpr uintptr_t SuperSimController=0xf8ce5b8,SuperSimVtable=0xb7d8980,SuperSimModeGetter=0x3754ce0;
}
