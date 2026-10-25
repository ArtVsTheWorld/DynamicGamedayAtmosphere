#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <intrin.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <string>
#include <vector>
#include "AtmosphereRules.h"
#include "ExcitementRules.h"
#include "ArrivalRules.h"
#include "DepartureModel.h"
#include "MatchLifecycle.h"
#include "ProductVersion.h"
#include "GameBindings.h"
#include "LogFiles.h"
#include "CrowdVisibility.h"
#include "SeatMapping.h"
#include "CrowdDispatch.h"
#include "CrowdPartResolver.h"
#include "CrowdAudioRuntime.h"
#include "ChantAudioModel.h"
#include "ChantMixerRuntime.h"
#include "CrowdIntensityRuntime.h"
#include "CrowdChangeWindow.h"
#include "SuperSimState.h"
#include "CoreAtmosphere.h"
#include "ReactionAudioRuntime.h"
#include "ResumeBridge.h"
#include "AttendanceTimeline.h"
#include "ReplayUpdateHook.h"
#include "ReplayPresentation.h"

using A=uintptr_t;
extern "C" void CrowdTick();
namespace {
A image=0; HMODULE self=nullptr; std::filesystem::path folder,pluginsFolder,logsFolder;
std::mutex logMutex;
std::atomic<int> logLimitMB{2};
std::atomic<bool> debugLog{false};
std::mutex historyMutex;attendance::History gameHistory;attendance::MatchLifecycle matchLifecycle;
std::atomic<bool> shownComeback{false},shownUpset{false},contextReady{false};
std::atomic<float> audioEmphasis{1};
std::mutex atmosphereMutex;
attendance::energy::Tracker playTracker;
attendance::core::Model coreAtmosphere;
std::shared_ptr<const attendance::core::Settings> coreSettings=std::make_shared<attendance::core::Settings>();
std::shared_ptr<const attendance::energy::Settings> retentionEventSettings=std::make_shared<attendance::energy::Settings>();
std::atomic<float> crowdBoost{0};
std::atomic<bool> shownPostgameCarry{false},shownEruption{false};
std::atomic<int> liveDown{-2},liveOffense{-1},liveHomeTimeouts{-1};
std::atomic<unsigned> attendanceSeed{0};
std::atomic<bool> playBindingsAvailable{false};
int pendingLateExits=0;A atmosphereMatch=0;unsigned long long lastAtmosphereTick=0;
std::atomic<float> chantReduction{50};
attendance::arrivals::Model arrivalModel;
attendance::CrowdChangeWindow crowdChangeWindow;
attendance::arrivals::Halftime halftimeModel;
attendance::departures::Model departureModel;
std::atomic<int> arrivalEnabled{1},arrivalStart{70},arrivalLateStart{50},arrivalFinish{25},arrivalLateFinish{75},arrivalMissing{0},halftimeExtra{100},shownHalftime{0},shownArriving{0};
std::atomic<int> arrivalBigBonus{20},arrivalWeatherPenalty{10},arrivalPlayoffStart{65},arrivalChampionshipStart{75},arrivalFcsPenalty{10};
std::atomic<int> arrivalModerateStart{60},arrivalModerateFinish{50};
std::atomic<bool> arrivalReconcilePending{false};
std::atomic<bool> visibilityRetry{false},captureArrivalBaseline{false};
std::atomic<bool> pregameRendererRetry{false};
std::atomic<bool> postgameRendererRetry{false};
std::atomic<bool> superSimBinding{false},shownSuperSim{false};
double simulationRecording=-1;
unsigned visibilityGeneration=0;
std::atomic<float> shownAmbienceLift{0},shownOpeningWeight{0},shownAudioLimit{4};
std::atomic<unsigned long long> shownDecisiveUntil{0};
std::atomic<int> request{0};
std::atomic<unsigned long long> ticks{0};
std::vector<A> entities;
std::mutex entityMutex;
std::atomic<unsigned> entityGeneration{0};
std::atomic<bool> refreshEntities{false};
std::shared_ptr<const attendance::Settings> settings=std::make_shared<attendance::Settings>();
std::atomic<int> mode{0}; // 0 automatic; 1 manual hold; 2 restored/disabled.
std::atomic<int> shownQuarter{0},shownScore0{0},shownScore1{0},shownPhase{0};
std::atomic<int> shownProfile{1},shownRank0{0},shownRank1{0},shownBowl{127},shownPrecipitation{0};
std::atomic<bool> shownRivalry{false},shownPlayoff{false},shownFcs{false};
std::atomic<bool> shownNeutral{false};
std::atomic<int> shownTeam0{-1},shownTeam1{-1};
std::atomic<float> shownSeconds{0},shownCurrent{0},shownTarget{0};
float currentLoss=0,highestTarget=0;
int lastApplied=-1,lastQuarter=0,lastScore0=0,lastScore1=0;
float lastClock=0;
A lastMatch=0;
unsigned lastGeneration=0;
unsigned long long lastDecision=0,lastFrame=0;
std::atomic<bool> enabled{false};
thread_local bool applying=false;
thread_local bool thinning=false;
thread_local bool reconcilingArrivals=false,capturingBaseline=false;
thread_local unsigned int removalLimit=0;
std::atomic<int> activeLoss{0};
std::atomic<bool> collectingStartingCrowd{true};
struct StartingCrowd {
    A masks=0;unsigned short parts=0;std::vector<unsigned short> hidden,temporary,nativeSeen,nativeHidden;
    A scene=0;unsigned captureToken=0;bool baselineComplete=false;attendance::seats::Pattern selection;
};
std::mutex crowdMutex;
std::unordered_map<A,StartingCrowd> startingCrowds;
std::atomic<bool> seatPatternPending{false};
std::atomic<unsigned> shownSeatPatterns{0},shownSeatFallbacks{0};
unsigned captureSequence=0;thread_local unsigned activeCaptureToken=0;
std::atomic<bool> replayBinding{false},exactVisibility{false},replaySuspendsAtmosphere{false};
std::atomic<int> replayLoss{-1};
bool replayWasActive=false,replayRestorePending=false;
std::mutex timelineMutex;attendance::timeline::Archive attendanceArchive;
std::shared_ptr<const attendance::resume::Signal> resumeSignal=std::make_shared<attendance::resume::Signal>();
attendance::resume::Tracker resumeTracker;
bool bootstrapPending=false;double lastRecordingTime=-1;
std::atomic<int> shownResumeSource{0}; // 0 ordinary, 1 estimated, 2 saved history.
thread_local unsigned int calls=0,changed=0,verifiedCalls=0;
std::atomic<unsigned> shownVisibilityCalls{0},shownVisibilityControllers{0};
std::atomic<unsigned long long> shownOriginalFans{0},shownVisibleFans{0},shownExpectedFans{0},shownNativeExtra{0},shownOtherExtra{0},shownExtraVisible{0};
std::atomic<bool> visibilityMeasured{false};
thread_local std::set<A> observed;
using SetVisibility=void(__fastcall*)(A,unsigned int,unsigned int,bool);
using SetPeriod=void(__fastcall*)(A,int);
using AttendanceUpdate=void(__fastcall*)(A);
AttendanceUpdate originalAttendanceUpdate=nullptr;
using PresentationUpdate=void(__fastcall*)(A,float);
PresentationUpdate originalPresentationUpdate=nullptr;
using ReplayUpdate=void(__fastcall*)(A);
ReplayUpdate originalReplayUpdate=nullptr;
std::mutex presentationMutex;
std::atomic<unsigned long long> replayCallbacks{0};
void __fastcall replayUpdate(A entity);
bool guardedDecision();
using MuseLogicEvaluate=bool(__fastcall*)(A,A);
using MuseComparisonValue=A(__fastcall*)(A,A);
MuseLogicEvaluate originalMuseLogic=nullptr;
MuseComparisonValue originalMuseComparison=nullptr;
std::atomic<bool> replayPresentationInstalled{false},replayPresentationFailed{false};
std::atomic<bool> extraReplays{true},routineInsets{true},bigPlayReplays{true},defensiveReplays{true};
std::atomic<int> replayMinimumYards{15};
std::atomic<unsigned long long> insetEligibilityChanges{0},fullEligibilityChanges{0},defensiveRouteChanges{0};
thread_local attendance::replaypresentation::ThreadState replaySelection;
SetPeriod originalPeriod=nullptr;
SetVisibility originalSet=nullptr;
unsigned long long restoreAt=0;
attendance::audio::Runtime crowdAudio;
attendance::audio::Update originalAudioUpdate=nullptr;
std::atomic<bool> audioHookInstalled{false},audioSetting{true};
std::atomic<int> audioReductionPercent{20},audioReductionStart{80};
attendance::chants::Runtime chantAudio;
attendance::chants::Runtime crowdBoostAudio,ambienceAudio;
attendance::mix::Continuity audioContinuity;
std::atomic<bool> chantAvailable{false};
std::atomic<int> chantStrength{100};
std::atomic<A> audioMatch{0};
std::atomic<unsigned> audioEpoch{1};
attendance::sustained::Runtime sustainedAudio;
attendance::cheer::Runtime positiveCheer;
std::atomic<bool> cheerAvailable{false};
using ReactionUpdate=void(__fastcall*)(A,A);ReactionUpdate originalReactionUpdate=nullptr;
attendance::sustained::IntensityRuntime crowdIntensity;
std::shared_ptr<const attendance::sustained::Intent> sustainedIntent=std::make_shared<attendance::sustained::Intent>();
std::atomic<bool> swellAvailable{false},intensityAvailable{false},audioAudible{false};
std::atomic<bool> pauseBinding{false};
std::atomic<unsigned long long> lastAudibleUpdate{0};
using IntensityUpdate=void(__fastcall*)(A);
IntensityUpdate originalIntensityUpdate=nullptr;
attendance::sustained::Api::Send originalCrowdSend=nullptr;
attendance::audio::Wanted audioWanted();
void audioNote(const char* message);
void sustainedNote(const char* message);
bool nativeUnpaused(){
    A pause=0;unsigned char paused=1;SIZE_T copied=0;
    return pauseBinding&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(image+attendance::bindings::PauseFlag),&pause,sizeof(pause),&copied)&&copied==sizeof(pause)&&pause>=0x10000&&
        ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(pause),&paused,1,&copied)&&copied==1&&paused==0;
}
attendance::sustained::Intent wantedSwell(){
    auto intent=*std::atomic_load(&sustainedIntent);auto wanted=audioWanted();auto c=std::atomic_load(&coreSettings);
    auto now=GetTickCount64(),audibleAt=lastAudibleUpdate.load();
    // Match context alone stays valid in the pause menu and Instant Replay.
    if(!wanted.active||mode!=0||!c->enabled||!audioAudible||now<audibleAt||now-audibleAt>750||
       !nativeUnpaused()||!attendance::sustained::fresh(intent,wanted.match,wanted.epoch,now))intent.active=false;
    return intent;
}
void __fastcall intensityUpdate(A entity){
    originalIntensityUpdate(entity);
    auto wanted=audioWanted();
    try {crowdIntensity.tick(entity,wantedSwell(),wanted.match,wanted.epoch,GetTickCount64());}
    catch(...){crowdIntensity.fail();audioNote("intensity-fault: temporary excitement stopped; native output will be restored on its next update");}
}
void __fastcall reactionUpdate(A entity,A frame){
    originalReactionUpdate(entity,frame);
    try {positiveCheer.tick(entity,wantedSwell(),GetTickCount64());}
    catch(...){positiveCheer.fail();audioNote("cheer-fault: further added cheers disabled");}
}
A __fastcall crowdSend(A manager,A result,A event){
    A returned=originalCrowdSend(manager,result,event);
    sustainedAudio.observeNativeStop(manager,event);
    return returned;
}
A __fastcall audioUpdate(A owner,A a,A b,A c){
    A result=crowdAudio.tick(owner,a,b,c);
    int volume=0;SIZE_T count=0;
    bool audible=owner>=0x10040&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(owner-0x40+attendance::bindings::CrowdSlider),&volume,4,&count)&&count==4&&volume>0&&volume<=100;
    audioAudible=audible;lastAudibleUpdate=GetTickCount64();
    if(swellAvailable)try {sustainedAudio.tick(owner,wantedSwell(),GetTickCount64());}
    catch(...){sustainedAudio.fail();audioNote("swell-fault: further starts disabled; cleanup will retry on the next audio update");}
    if(chantAvailable){
        auto wanted=audioWanted();
        // activeLoss is the existing no-refill attendance checkpoint. Replay
        // scores/clocks never select a different chant target.
        try {
            float chantCut=attendance::chants::attendancePercent(wanted.remaining,chantStrength,chantReduction,wanted.emphasis);
            chantAudio.tick(owner,wanted.active,attendance::volume::nativeDb(1-chantCut*0.01f));
            // Additive Gain on Crowd only. The user-volume packet stays in its
            // supported 0..1 range. Chants inherit this parent once, not twice.
            auto energy=std::atomic_load(&coreSettings);
            bool active=wanted.active&&audible&&energy->enabled&&mode==0;
            float normal=attendance::core::normalLimit(*energy);
            float limit=attendance::core::callbackLimit(*energy,shownAudioLimit.load(),GetTickCount64(),shownDecisiveUntil.load());
            float parent=(std::clamp)(crowdBoost.load(),0.0f,limit);
            float bed=attendance::mix::ambienceForTotal(parent,crowdBoostAudio.db(),shownAmbienceLift.load(),limit);
            // Reduce the child first when a parent surge needs its headroom.
            ambienceAudio.tick(owner,active,bed,false,false,13);
            parent=attendance::mix::parentGain(parent,ambienceAudio.db(),limit);
            crowdBoostAudio.tick(owner,active,parent,false,false,9,parent<=normal&&crowdBoostAudio.db()>normal);
        }
        catch(...){chantAvailable=false;audioNote("chant-fault: chant updates stopped after exception");}
    }
    return result;
}

bool signatures=false;
bool read(A p,void* out,size_t n);
void __fastcall periodChanged(A manager,int next);
std::vector<unsigned char> cleanEntry(A rva){
    if(rva==0x64afda0)return {0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
    if(rva==0x5d024c0)return {0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0x99,0x30,0x01,0,0,0x48,0x89,0x6c,0x24,0x30};
    if(rva==attendance::bindings::AudioUpdate)return {0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x48,0x8b,0x89,0x38,0x02,0,0};
    if(rva==attendance::bindings::IntensityUpdate)return {0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x56,0x57,0x41,0x56,0x48,0x83,0xec,0x20};
    if(rva==attendance::bindings::SwellSend)return {0x4c,0x8b,0xdc,0x55,0x41,0x56,0x49,0x8d,0x6b,0xd8,0x48,0x81,0xec,0x18,1,0,0};
    if(rva==attendance::bindings::MuseLogicEvaluate)return {0x48,0x8b,0xc4,0x55,0x48,0x83,0xec,0x50,0x44,0x8b,0x41,0x48,0x40,0xb5,0x01};
    if(rva==attendance::bindings::MuseComparisonValue)return {0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x4c,0x89,0x74,0x24,0x20};
    return {};
}
struct Patch {A site=0;void* trampoline=nullptr;std::vector<unsigned char> original,replacement;};
std::vector<Patch> patches;
void jump(unsigned char* p,A target){p[0]=0xff;p[1]=0x25;memset(p+2,0,4);memcpy(p+6,&target,8);}
bool makePatch(A rva,void* detour,void** original,size_t length){
    Patch p;p.site=image+rva;p.original.resize(length);p.replacement.assign(length,0x90);
    if(!read(p.site,p.original.data(),length))return false;
    size_t allocated=rva==attendance::bindings::ReplayUpdate?attendance::replayhook::TrampolineBytes:length+14;
    p.trampoline=VirtualAlloc(nullptr,allocated,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);if(!p.trampoline)return false;
    auto clean=cleanEntry(rva);
    // Copy only the complete, verified prologue for this hook.
    if(!clean.empty()&&clean.size()!=length)return false;
    if(rva==attendance::bindings::ReplayUpdate){
        if(length!=attendance::replayhook::SourceBytes||std::memcmp(p.original.data(),attendance::replayhook::Prefix.data(),length)){VirtualFree(p.trampoline,0,MEM_RELEASE);return false;}
        auto code=attendance::replayhook::trampoline(image+attendance::bindings::ReplayManager,p.site+length);std::memcpy(p.trampoline,code.data(),code.size());
    }else {memcpy(p.trampoline,clean.empty()?p.original.data():clean.data(),length);jump(static_cast<unsigned char*>(p.trampoline)+length,p.site+length);}
    DWORD old=0;if(!VirtualProtect(p.trampoline,allocated,PAGE_EXECUTE_READ,&old)){VirtualFree(p.trampoline,0,MEM_RELEASE);return false;}
    FlushInstructionCache(GetCurrentProcess(),p.trampoline,allocated);jump(p.replacement.data(),reinterpret_cast<A>(detour));
    *original=p.trampoline;patches.push_back(std::move(p));return true;
}
bool commitPatches(){
    // Complete prologues; the replay updater expands its verified RIP-relative load.
    std::vector<HANDLE> frozen;frozen.reserve(4096);bool okay=true;
    std::vector<DWORD> protection(patches.size());
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(snapshot==INVALID_HANDLE_VALUE)return false;
    THREADENTRY32 e{};e.dwSize=sizeof(e);
    if(Thread32First(snapshot,&e))do{
        if(e.th32OwnerProcessID!=GetCurrentProcessId()||e.th32ThreadID==GetCurrentThreadId())continue;
        HANDLE t=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT,FALSE,e.th32ThreadID);
        if(!t){if(GetLastError()==ERROR_INVALID_PARAMETER)continue;okay=false;break;}
        if(SuspendThread(t)==DWORD(-1)){CloseHandle(t);okay=false;break;}frozen.push_back(t);
    }while(Thread32Next(snapshot,&e));
    CloseHandle(snapshot);
    if(okay)for(HANDLE t:frozen){
        CONTEXT c{};c.ContextFlags=CONTEXT_CONTROL;if(!GetThreadContext(t,&c)){okay=false;break;}
        for(const auto& p:patches)if(c.Rip>=p.site&&c.Rip<p.site+p.original.size()){
            c.Rip=reinterpret_cast<A>(p.trampoline)+c.Rip-p.site;if(!SetThreadContext(t,&c))okay=false;
        }
    }
    if(okay)for(const auto& p:patches){unsigned char actual[32]{};if(p.original.size()>sizeof(actual)||!read(p.site,actual,p.original.size())||memcmp(actual,p.original.data(),p.original.size()))okay=false;}
    // Make every target writable before any code is changed; roll back protections on failure.
    size_t prepared=0;
    if(okay)for(size_t i=0;i<patches.size();++i){if(!VirtualProtect(reinterpret_cast<void*>(patches[i].site),patches[i].original.size(),PAGE_EXECUTE_READWRITE,&protection[i])){okay=false;break;}++prepared;}
    if(okay)for(const auto& p:patches){memcpy(reinterpret_cast<void*>(p.site),p.replacement.data(),p.replacement.size());FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(p.site),p.replacement.size());}
    for(size_t i=0;i<prepared;++i){DWORD old=0;VirtualProtect(reinterpret_cast<void*>(patches[i].site),patches[i].original.size(),protection[i],&old);}
    for(HANDLE t:frozen){ResumeThread(t);CloseHandle(t);}return okay;
}

void log(const char* event,const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    const std::string kind=event;
    if(!debugLog&&kind!="starting"&&kind!="ready"&&kind!="inactive"&&kind!="fault"&&kind!="quarter-break"&&
       kind!="two-minute"&&kind!="departures"&&kind!="arrivals"&&kind!="halftime"&&kind!="opening"&&kind!="eruption"&&
       kind!="matchup"&&kind!="roar"&&kind!="late-score"&&kind!="protection"&&kind!="checkpoint-skipped"&&kind!="new-game"&&kind!="requested"&&
       kind!="resumed-game"&&kind!="replay"&&kind!="history"&&kind!="replay-unavailable"&&kind!="seating"&&
       kind.find("inactive")==std::string::npos&&!(kind=="audio"&&message.find("fault")!=std::string::npos))return;
    if(!debugLog&&kind=="arrivals"&&message.find("starting at")==std::string::npos&&message.find("complete")==std::string::npos)return;
    static std::string previous;static unsigned long long previousAt=0;
    if(message==previous&&GetTickCount64()-previousAt<5000)return;
    previous=message;previousAt=GetTickCount64();
    auto path=logsFolder/L"DynamicGamedayAtmosphere.log";
    SYSTEMTIME local{};GetLocalTime(&local);char timestamp[40]{};
    sprintf_s(timestamp,"%04u-%02u-%02u %02u:%02u:%02u",local.wYear,local.wMonth,local.wDay,local.wHour,local.wMinute,local.wSecond);
    attendance::appendLog(path,std::string(timestamp)+" | "+message+"\n",static_cast<uintmax_t>(logLimitMB.load())*1024*1024);
}
std::string hex(A a){char s[32]{};sprintf_s(s,"0x%llx",static_cast<unsigned long long>(a));return s;}
bool read(A p,void* out,size_t n){SIZE_T done=0;return p>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&done)&&done==n;}
template<class T>T at(A p){T v{};read(p,&v,sizeof(v));return v;}
void observeWorldExit(){
    if(at<int>(image+0xd9e3818)==14&&at<int>(image+0xf3f5d48)==3)return;
    std::lock_guard<std::mutex> guard(historyMutex);
    // Recheck under the lock so a delayed worker observation cannot mark a
    // freshly joined game as belonging to the world that was just left.
    if(at<int>(image+0xd9e3818)!=14||at<int>(image+0xf3f5d48)!=3)matchLifecycle.leave();
}
attendance::replaypresentation::View replayPresentationView(){
    using namespace attendance::bindings;
    return {read,{image+MuseLogicVtable,image+MuseComparisonVtable,image+MuseConstantVtable,image+MuseFunctionVtable,image+MuseActorFunctionVtable}};
}
bool replayPresentationAllowed(){
    if(!enabled||mode==2||!extraReplays||!replayPresentationInstalled||replayPresentationFailed||!pauseBinding||!replayBinding)return false;
    if(at<int>(image+0xd9e3818)!=14||at<int>(image+0xf3f5d48)!=3)return false;
    A states=at<A>(image+attendance::bindings::GameStates);
    if(at<A>(states)!=image+attendance::bindings::GameStatesVtable||at<int>(states+0xa0)!=4)return false;
    A configuration=at<A>(image+0xf6379e0),wrapper=at<A>(image+attendance::bindings::MatchWrapper);
    A match=at<A>(wrapper+16),manager=at<A>(image+attendance::bindings::ReplayManager);
    unsigned char online=1,active=1;
    return configuration&&read(configuration+0x84,&online,1)&&online==0&&
        at<A>(match)==image+attendance::bindings::MatchVtable&&at<A>(manager)==image+attendance::bindings::ReplayVtable&&
        read(manager+0x38,&active,1)&&active==0&&nativeUnpaused();
}
bool writeReplayBoolean(A address,bool value=true){
    // Only an existing native eligibility result is written. A failed write
    // disables these additions without changing original presentation calls.
    __try{*reinterpret_cast<unsigned char*>(address)=static_cast<unsigned char>(value);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){replayPresentationFailed=true;return false;}
}
bool __fastcall museLogicEvaluate(A node,A context){
    auto view=replayPresentationView();
    attendance::replaypresentation::Scope scope(replaySelection,view,node,
        replaySelection.depth==0&&replayPresentationAllowed(),bigPlayReplays.load(),routineInsets.load(),defensiveReplays.load());
    bool valid=originalMuseLogic(node,context);
    // A Logic Eval's return means its cached variant is valid, not that its
    // Boolean is true. The native parent reads this cache after Eval returns.
    if(valid&&node==replaySelection.selection.insetGate&&extraReplays&&routineInsets&&!replayPresentationFailed&&
       view.falseBoolean(node+0x38)&&writeReplayBoolean(node+0x38))++insetEligibilityChanges;
    // Decline only complete authored shortcut selectors, never the underlying
    // sack/tackle facts. Standard Flow then chooses the native sack replay, or
    // the routine run/pass inset instead of the replay-less TFL cutaway.
    if(valid&&node==replaySelection.selection.denyRoot&&extraReplays&&defensiveReplays&&!replayPresentationFailed&&
       view.trueBoolean(node+0x38)&&writeReplayBoolean(node+0x38,false))++defensiveRouteChanges;
    return valid;
}
A __fastcall museComparisonValue(A node,A output){
    A result=originalMuseComparison(node,output);
    if(node==replaySelection.selection.fullComparison&&extraReplays&&bigPlayReplays&&!replayPresentationFailed){
        auto view=replayPresentationView();
        if(result&&view.falseBoolean(result)&&view.fullEligible(node,replayMinimumYards.load())&&writeReplayBoolean(result))++fullEligibilityChanges;
    }
    return result;
}
attendance::audio::Wanted audioWanted(){
    attendance::audio::Wanted wanted;wanted.epoch=audioEpoch.load();wanted.match=audioMatch.load();
    // Replay may use different clocks. Ownership and match identity gate audio;
    // its target comes from the attendance checkpoint, not the replay's score.
    A wrapper=at<A>(image+attendance::bindings::MatchWrapper),actual=at<A>(wrapper+16),configuration=at<A>(image+0xf6379e0);
    wanted.active=enabled&&audioSetting&&mode!=2&&std::atomic_load(&settings)->enabled&&wanted.match&&
        wanted.match==actual&&configuration&&at<unsigned char>(configuration+0x84)!=1&&at<A>(actual)==image+0xb4781e8&&at<int>(image+0xd9e3818)==14&&at<int>(image+0xf3f5d48)==3;
    wanted.remaining=(100-(std::clamp)(activeLoss.load(),0,100))*0.01f;
    wanted.arrivalFraction=(100-(std::clamp)(arrivalMissing.load(),0,100))*0.01f;
    wanted.strength=audioReductionPercent.load()*0.01f;wanted.reductionStart=audioReductionStart.load()*0.01f;wanted.emphasis=audioEmphasis.load();
    // Opening hype softens only temporary arrivals, never permanent departures.
    wanted.arrivalFraction+=(1-wanted.arrivalFraction)*(std::clamp)(shownOpeningWeight.load(),0.0f,1.0f);
    return wanted;
}
void audioNote(const char* message){log("audio",message);}
void sustainedNote(const char* message){
    std::string text=message;
    if(text=="big-game opening")text="Opening roar started; sustained through the intro and opening possession.";
    else if(text=="overtime roar")text="Continuous overtime roar started.";
    else if(text=="winning celebration")text="Winning celebration roar started.";
    else if(text.find("fading out")!=std::string::npos)text="Added crowd roar fading out.";
    log("roar",text);
}

bool entityValid(A p){return at<A>(p)==image+0xb47ec48&&at<A>(at<A>(p+0x38)+8)==image+0xf502b30;}
void __fastcall presentationUpdate(A states,float delta){
    originalPresentationUpdate(states,delta);
    // Native post-update runs both presentation and gameplay. Using it alone
    // avoids the gameplay hook owned by Situational Practice and Call Sheet.
    if(enabled&&states==at<A>(image+attendance::bindings::GameStates)&&at<A>(states)==image+0xb479070)CrowdTick();
}
void __fastcall attendanceUpdate(A entity){
    originalAttendanceUpdate(entity);
    // Observe controllers only after their native scene update has completed.
    // This discovers a freshly loaded crowd without waiting for a heap scan.
    if(!enabled||applying||!entityValid(entity))return;
    {std::lock_guard<std::mutex> guard(entityMutex);
        if(std::find(entities.begin(),entities.end(),entity)==entities.end()&&entities.size()<8){
            entities.push_back(entity);std::sort(entities.begin(),entities.end());++entityGeneration;
            visibilityRetry=true;
        }
    }
    A states=at<A>(image+attendance::bindings::GameStates);
    int phase=at<int>(states+0xa0);
    if(at<A>(states)==image+attendance::bindings::GameStatesVtable&&(visibilityRetry||seatPatternPending)){
        // The renderer can appear after the intro's one-shot arrival window.
        // Queue that SAME setup for presentation's existing post-update point.
        // Never re-enter the scene update while its native caller is unwinding.
        if(phase==6)pregameRendererRetry=true;
        else if(phase==7||(phase==8&&at<int>(at<A>(image+attendance::bindings::ClockManager)+8)==6))postgameRendererRetry=true;
    }
}
bool option(unsigned key,int width,int& value);
bool gameState(attendance::State& s,A& match){
    if(at<int>(image+0xd9e3818)!=14||at<int>(image+0xf3f5d48)!=3)return false;
    A configuration=at<A>(image+0xf6379e0);
    if(!configuration||at<unsigned char>(configuration+0x84)==1)return false;
    A states=at<A>(image+attendance::bindings::GameStates),wrapper=at<A>(image+attendance::bindings::MatchWrapper),clockManager=at<A>(image+attendance::bindings::ClockManager);
    match=at<A>(wrapper+16);
    if(at<A>(match)!=image+0xb4781e8||at<A>(states)!=image+0xb479070||at<A>(clockManager)!=image+0xb4787a8)return false;
    A gameClock=at<A>(clockManager+0x28);
    if(!read(clockManager+8,&s.quarter,4)||!read(states+0xa0,&s.phase,4)||!read(match+0x1d8,&s.homeScore,4)||!read(match+0x1dc,&s.awayScore,4))return false;
    bool clockReady=read(gameClock+0x28,&s.seconds,4)&&read(gameClock+0x2c,&s.quarterSeconds,4);
    attendance::normalizeFinalPresentation(s);
    if(s.phase==6){
        if(!clockReady||!std::isfinite(s.quarterSeconds)||s.quarterSeconds<60||s.quarterSeconds>1200){
            int minutes=0;if(!option(0x9f1b2500,2,minutes)||minutes<1||minutes>20)return false;
            s.quarterSeconds=static_cast<float>(minutes*60);
        }
        if(!attendance::normalizePregame(s))return false;
    }else if(!clockReady)return false;
    else if(attendance::untimedPeriod(s)){
        // Untimed OT can expose a zero-length clock. Normalize our snapshot
        // with the configured regulation length, never the game's clock.
        int minutes=0;if(!option(0x9f1b2500,2,minutes)||minutes<1||minutes>20)return false;
        if(!attendance::normalizeUntimedPeriod(s,static_cast<float>(minutes*60)))return false;
    }
    return attendance::valid(s);
}
bool offscreenSimulation(const attendance::State& s){
    return superSimBinding&&attendance::simulation::offscreen(read,image,s.phase);
}
bool bytes(A rva,const std::vector<unsigned char>& expected){
    std::vector<unsigned char> got(expected.size());if(!read(image+rva,got.data(),got.size()))return false;
    if(got==expected)return true;
    return false; // A foreign detour is never silently replaced or chained.
}
struct ReplaySnapshot {bool valid=false,active=false;double playback=0,recording=0;};
ReplaySnapshot replaySnapshot(){
    ReplaySnapshot s;if(!replayBinding)return s;
    A manager=at<A>(image+attendance::bindings::ReplayManager);
    if(at<A>(manager)!=image+attendance::bindings::ReplayVtable)return s;
    unsigned char active=0;
    if(!read(manager+0x38,&active,1)||active>1||!read(manager+0x40,&s.playback,8)||!read(manager+0x1030,&s.recording,8)||
        !std::isfinite(s.playback)||!std::isfinite(s.recording)||s.playback<0||s.recording<0||s.playback>86400||s.recording>86400)return s;
    s.valid=true;s.active=active!=0;return s;
}
bool swellBindings(){
    using namespace attendance::bindings;
    struct Check {A rva;std::vector<unsigned char> code;};
    const Check checks[]={
        {SwellLookup,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x6c,0x24,0x10}},
        {SwellConstruct,{0x40,0x53,0x57,0x41,0x54,0x41,0x55,0x41,0x57}},
        {SwellDestroy,{0x41,0x56,0x48,0x83,0xec,0x20}},
        {SwellSetInt,{0x48,0x89,0x5c,0x24,0x10,0x57,0x48,0x83,0xec,0x20}},
        {SwellSend,{0x4c,0x8b,0xdc,0x55,0x41,0x56,0x49,0x8d,0x6b,0xd8,0x48,0x81,0xec,0x18,1,0,0}},
        {SwellQuery,{0x40,0x53,0x48,0x83,0xec,0x30}},
        {SwellQueryGet,{0x48,0x89,0x5c,0x24,8,0x57,0x48,0x83,0xec,0x30}},
        {SwellQueryDestroy,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18}},
        {SwellWeakRelease,{0xb8,0xff,0xff,0xff,0xff,0xf0,0x0f,0xc1,0x41,8,0x83,0xf8,1}},
        {SwellArenaInit,{0x40,0x53,0x48,0x83,0xec,0x20}},
        {SwellArenaDestroy,{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20}},
        {SwellManagerGetter,{0x48,0x8b,0x41,0x50,0xc3}}
    };
    for(const auto& check:checks)if(!bytes(check.rva,check.code))return false;
    return true;
}

bool maskLayout(A crowd,A& renderer,A& begin,size_t& count,A& parts){
    __try {
        renderer=*reinterpret_cast<A*>(crowd+0x9f0);parts=*reinterpret_cast<A*>(crowd+0x908);
        if(*reinterpret_cast<A*>(renderer)!=image+0xb7db228)return false;
        begin=*reinterpret_cast<A*>(renderer+0x188);A end=*reinterpret_cast<A*>(renderer+0x190);
        if(begin<0x10000||end<begin||(end-begin)%2||end-begin>0x200000)return false;
        count=(end-begin)/2;return count>0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool currentMask(A begin,unsigned agent,unsigned short& mask){
    __try {mask=reinterpret_cast<const unsigned short*>(begin)[agent];return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Runs after the native baseline pass, outside the per-fan callback. Build one
// immutable order per renderer, then share it across every attendance purpose.
void prepareSeatPatterns(){
    std::lock_guard<std::mutex> guard(crowdMutex);bool pending=false;unsigned ready=0,fallback=0;
    for(auto cursor=startingCrowds.begin();cursor!=startingCrowds.end();){
        auto& item=*cursor;
        auto& base=item.second;
        if(at<A>(base.scene)!=image+attendance::bindings::SceneCrowdVtable||at<A>(base.scene+0x9f0)!=item.first||
           at<A>(item.first)!=image+attendance::bindings::CrowdRendererVtable||at<A>(item.first+0x188)!=base.masks||
           at<A>(item.first+0x190)!=base.masks+base.hidden.size()*2){cursor=startingCrowds.erase(cursor);continue;}
        ++cursor;
        if(base.selection.policy==attendance::seats::Policy::Pending){
            // Some native parts belong only to certain fan models. Requiring
            // every part on every fan would leave those scenes pending forever.
            if(!base.baselineComplete||!base.parts){
                // Tiny sideline draws may never be visited by this attendance
                // controller. Leave their unverified native seats alone, and
                // do not let them keep a ready seated stadium in retry mode.
                if(base.hidden.size()>=512)pending=true;
                continue;
            }
            std::vector<attendance::seats::Position> coordinates;
            try{
                if(attendance::seats::positions(read,image,base.scene,item.first,base.masks,base.hidden.size(),coordinates))
                    base.selection=attendance::seats::build(coordinates,base.hidden,base.parts,attendanceSeed.load());
                else base.selection.policy=attendance::seats::Policy::Scattered;
            }catch(const std::exception&){base.selection={};base.selection.policy=attendance::seats::Policy::Scattered;}
            if(base.selection.policy==attendance::seats::Policy::Position)
                log("seating","Crowd seating pattern ready: fuller near the field, uneven groups and scattered fans farther back.");
            else if(base.hidden.size()>=512)
                log("seating","Seat positions could not be verified for this crowd; using the standard scattered pattern.");
        }
        if(base.selection.policy==attendance::seats::Policy::Position)++ready;
        else if(base.selection.policy==attendance::seats::Policy::Scattered)++fallback;
    }
    seatPatternPending=pending;shownSeatPatterns=ready;shownSeatFallbacks=fallback;
}
void __fastcall visibility(A crowd,unsigned int part,unsigned int agent,bool hidden) {
    if(applying)++calls;
    A renderer=0,begin=0,parts=0;size_t count=0;
    if(enabled&&maskLayout(crowd,renderer,begin,count,parts)&&agent<count){
        // The backend stores a hidden bit per resolved part, for each fan.
        unsigned bit=16;
        if(attendance::visibility::resolvePart(reinterpret_cast<attendance::visibility::PartResolver>(image+0x65047b0),parts,part,bit)){
            std::lock_guard<std::mutex> guard(crowdMutex);
            auto found=startingCrowds.find(renderer);
            const bool collecting=(collectingStartingCrowd.load()&&!applying)||capturingBaseline;
            if((applying||collecting||activeLoss>0||arrivalMissing>0||replayLoss>=0)&&(found==startingCrowds.end()||found->second.scene!=crowd||found->second.masks!=begin||found->second.hidden.size()!=count)){
                StartingCrowd baseline;baseline.masks=begin;baseline.scene=crowd;baseline.hidden.resize(count);baseline.temporary.resize(count);baseline.nativeSeen.resize(count);
                if(!read(begin,baseline.hidden.data(),count*2)){log("baseline-unavailable","Crowd write skipped");return;}
                baseline.nativeHidden=baseline.hidden;
                found=startingCrowds.insert_or_assign(renderer,std::move(baseline)).first;
                seatPatternPending=true;
                if(!applying)visibilityRetry=true;
                log("starting-crowd","renderer="+hex(renderer)+" fans="+std::to_string(count)+" preserved existing empty seats");
            }
            if(found!=startingCrowds.end()&&found->second.masks==begin&&agent<found->second.hidden.size()){
                if(capturingBaseline)found->second.captureToken=activeCaptureToken;
                found->second.parts|=static_cast<unsigned short>(1u<<bit);
                unsigned short flag=static_cast<unsigned short>(1u<<bit);
                auto& latest=found->second.nativeHidden[agent];
                latest=static_cast<unsigned short>((latest&~flag)|(hidden?flag:0));
                // The first native input for each part is authoritative even
                // if a newly loaded buffer initially contained empty defaults.
                bool firstNativeInput=!(found->second.nativeSeen[agent]&flag);
                if(firstNativeInput){
                    auto& original=found->second.hidden[agent];
                    original=static_cast<unsigned short>((original&~(1u<<bit))|(hidden?(1u<<bit):0u));
                    found->second.nativeSeen[agent]|=flag;
                }
                const bool restore=mode==2||(applying&&!thinning);
                const auto& selection=found->second.selection;
                if(!capturingBaseline&&selection.policy!=attendance::seats::Policy::Pending&&(!collecting||activeLoss>0||arrivalMissing>0||replayLoss>=0)&&(applying||!restore)){
                    unsigned short current=0;if(!currentMask(begin,agent,current)){if(applying)return;}
                    bool baseHidden=(found->second.hidden[agent]&(1u<<bit))!=0;
                    bool wasHidden=(current&(1u<<bit))!=0;
                    bool permanent=selection.departed(agent,activeLoss.load());
                    bool withheld=selection.departed(agent,arrivalMissing.load());
                    auto& owned=found->second.temporary[agent];bool wasTemporary=(owned&(1u<<bit))!=0;
                    bool result=attendance::hidePart(hidden,baseHidden,wasHidden,permanent||withheld,restore,reconcilingArrivals,wasTemporary,firstNativeInput);
                    int historical=replayLoss.load();
                    if(!restore&&(historical>=0||exactVisibility))result=attendance::historicalSelection(hidden,baseHidden,selection.departed(agent,historical>=0?historical:(std::max)(activeLoss.load(),arrivalMissing.load())));
                    if(withheld&&!permanent&&!baseHidden&&!hidden)owned|=static_cast<unsigned short>(1u<<bit);
                    else if(reconcilingArrivals&&!withheld)owned&=static_cast<unsigned short>(~(1u<<bit));
                    if(applying&&result&&!hidden)++changed;
                    hidden=result;
                }
                if(applying)++verifiedCalls;
            }
        }else if(applying)return;
    }else if(applying)return;
    originalSet(crowd,part,agent,hidden);
}
void resetMatch(){
    replayLoss=-1;exactVisibility=false;replaySuspendsAtmosphere=false;replayWasActive=false;replayRestorePending=false;bootstrapPending=false;lastRecordingTime=-1;shownResumeSource=0;
    crowdChangeWindow.reset();simulationRecording=-1;shownSuperSim=false;
    attendanceSeed=0;
    {std::lock_guard<std::mutex> guard(timelineMutex);attendanceArchive.active=0;}
    {std::lock_guard<std::mutex> guard(historyMutex);gameHistory.reset();arrivalModel.reset();halftimeModel.reset();departureModel.reset();matchLifecycle.reset();}
    {std::lock_guard<std::mutex> guard(atmosphereMutex);playTracker.reset();coreAtmosphere.reset();audioContinuity.reset();pendingLateExits=0;atmosphereMatch=0;}
    crowdBoost=0;shownEruption=shownPostgameCarry=false;
    arrivalMissing=0;arrivalReconcilePending=false;visibilityRetry=false;captureArrivalBaseline=false;pregameRendererRetry=postgameRendererRetry=false;shownAmbienceLift=shownOpeningWeight=0;shownAudioLimit=4;shownDecisiveUntil=0;visibilityGeneration=0;
    shownHalftime=shownArriving=0;shownNeutral=false;shownTeam0=shownTeam1=-1;
    shownVisibilityCalls=shownVisibilityControllers=0;shownOriginalFans=shownVisibleFans=shownExpectedFans=shownNativeExtra=shownOtherExtra=shownExtraVisible=0;visibilityMeasured=false;
    audioEmphasis=1;shownComeback=shownUpset=false;contextReady=false;
    shownRank0=shownRank1=shownPrecipitation=0;shownRivalry=shownPlayoff=shownFcs=false;
    {std::lock_guard<std::mutex> guard(crowdMutex);startingCrowds.clear();}
    seatPatternPending=false;shownSeatPatterns=shownSeatFallbacks=0;
    collectingStartingCrowd=true;activeLoss=0;currentLoss=highestTarget=0;lastApplied=0;lastMatch=0;lastQuarter=0;lastClock=0;
    shownCurrent=shownTarget=0;observed.clear();restoreAt=0;refreshEntities=true;
    audioMatch=0;++audioEpoch;
    std::shared_ptr<const attendance::sustained::Intent> quiet=std::make_shared<attendance::sustained::Intent>();std::atomic_store(&sustainedIntent,quiet);
}
void apply(bool thin,bool arrivals=false,bool captureOnly=false) {
    collectingStartingCrowd=false;
    std::vector<A> snapshot;{std::lock_guard<std::mutex> guard(entityMutex);snapshot=entities;}
    bool needCapture=(captureArrivalBaseline&&activeLoss==0)||seatPatternPending||visibilityGeneration!=entityGeneration.load();
    {std::lock_guard<std::mutex> guard(crowdMutex);needCapture=needCapture||startingCrowds.empty();}
    auto result=attendance::visibility::dispatch(snapshot,captureOnly,needCapture,[&](A entity){
        if(entityValid(entity))return true;refreshEntities=true;return false;
    },[&](A entity,bool baseline){
        if(!baseline)prepareSeatPatterns();
        calls=changed=verifiedCalls=0;applying=true;capturingBaseline=baseline;
        activeCaptureToken=baseline?++captureSequence:0;
        thinning=!baseline&&thin;reconcilingArrivals=!baseline&&arrivals;
        // The engine resolves controller -> scene crowds. Renderer readiness
        // is checked on each actual crowd in visibility(), before any write.
        originalAttendanceUpdate(entity);
        if(baseline){std::lock_guard<std::mutex> guard(crowdMutex);for(auto& item:startingCrowds)
            if(item.second.captureToken==activeCaptureToken)item.second.baselineComplete=true;
        }
        activeCaptureToken=0;
        applying=false;thinning=false;reconcilingArrivals=capturingBaseline=false;
        return attendance::visibility::Pass{calls,verifiedCalls,changed};
    });
    shownVisibilityControllers=result.controllers;shownVisibilityCalls=result.verified;
    if(captureOnly)prepareSeatPatterns();
    visibilityRetry=result.verified==0||seatPatternPending;
    if(visibilityRetry)refreshEntities=true;
    else if(captureOnly||arrivalMissing>0)captureArrivalBaseline=false;
    attendance::VisibilityCount population;bool measured=false;
    const int historical=replayLoss.load();
    const int measuredLoss=historical>=0?historical:(std::max)(activeLoss.load(),arrivalMissing.load());
    if(!captureOnly&&result.verified){
        std::lock_guard<std::mutex> guard(crowdMutex);
        for(const auto& item:startingCrowds){
            A renderer=item.first;const auto& base=item.second;
            if(at<A>(base.scene)!=image+attendance::bindings::SceneCrowdVtable||at<A>(base.scene+0x9f0)!=renderer||at<A>(renderer)!=image+0xb7db228||at<A>(renderer+0x188)!=base.masks||
               at<A>(renderer+0x190)!=base.masks+base.hidden.size()*2||!base.parts)continue;
            std::vector<unsigned short> current(base.hidden.size());if(!read(base.masks,current.data(),current.size()*2))continue;
            measured=true;
            for(size_t i=0;i<current.size();++i){
                if(base.selection.policy==attendance::seats::Policy::Pending)continue;
                attendance::countSelection(population,base.parts,base.hidden[i],current[i],base.nativeHidden[i],base.selection.departed(static_cast<unsigned>(i),measuredLoss));
            }
        }
    }
    if(!captureOnly){
        shownOriginalFans=population.original;shownVisibleFans=population.visible;shownExpectedFans=population.expected;
        shownNativeExtra=population.nativeExtra;shownOtherExtra=population.otherExtra;shownExtraVisible=population.extraVisible;
        visibilityMeasured=measured&&population.original>0;
    }
    std::string detail="controllers="+std::to_string(result.controllers)+" verified visibility calls="+std::to_string(result.verified)+" hidden overrides="+std::to_string(result.changed);
    if(visibilityMeasured)detail+="; measured "+std::to_string(shownVisibleFans.load())+" / "+std::to_string(shownOriginalFans.load())+" original crowd agents visible";
    if(visibilityMeasured)detail+="; selection expected "+std::to_string(population.expected)+" visible, additionally hidden by game="+std::to_string(population.nativeExtra)+", other hidden="+std::to_string(population.otherExtra);
    log(visibilityRetry?"visibility-waiting":"attendance-applied",std::string(captureOnly?"Starting crowd capture: ":"Crowd visibility update: ")+detail+(visibilityRetry?"; waiting for the scene's actual crowd renderer.":"."));
}
bool guardedApply(bool thin,bool arrivals=false,bool captureOnly=false) {
    __try {apply(thin,arrivals,captureOnly);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){applying=false;thinning=false;reconcilingArrivals=capturingBaseline=false;return false;}
}
void findEntities() {
    std::vector<A> found;
    const A needle=image+0xb47ec48;std::vector<unsigned char> buffer(0x100007);
    for(A addr=0x10000;addr<0x100000000;){
        MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(addr),&m,sizeof(m)))break;
        A start=reinterpret_cast<A>(m.BaseAddress),end=start+m.RegionSize;if(end<=addr)break;addr=end;
        if(m.State!=MEM_COMMIT||m.Type!=MEM_PRIVATE||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))||!(m.Protect&0xee))continue;
        end=(std::min)(end,A(0x100000000));
        for(A p=start;p<end;p+=0x100000){
            size_t n=(std::min)(size_t(0x100007),size_t(end-p));if(!read(p,buffer.data(),n))continue;
            for(size_t i=0;i+8<=n;i+=8){A value=0;memcpy(&value,buffer.data()+i,8);if(value==needle&&entityValid(p+i))found.push_back(p+i);}
        }
    }
    std::sort(found.begin(),found.end());found.erase(std::unique(found.begin(),found.end()),found.end());
    if(found.size()>8){log("discovery-skipped","Too many attendance entities; no candidates published");return;}
    {std::lock_guard<std::mutex> guard(entityMutex);if(found==entities)return;entities=found;++entityGeneration;}
    for(A e:found)log("entity",hex(e)+" asset="+hex(at<A>(e+0x38)));
}
bool install() {
    if(at<unsigned int>(image+0x3c)>0x1000)return false;
    if(!bytes(0x64afda0,cleanEntry(0x64afda0))||
       !bytes(0x5d024c0,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0x99,0x30,0x01,0,0,0x48,0x89,0x6c,0x24,0x30})||
       !bytes(0x65047b0,{0x4c,0x8b,0x91,0xa0,0,0,0,0x44,0x8b,0xca})||
       !bytes(0xa0ad090,{0x48,0x83,0xec,0x38,0x89,0x54,0x24,0x28})||
       !bytes(attendance::bindings::NeutralGetter,{0x48,0x83,0xec,0x28,0x48,0x8b,0x0d,0x15,0x63,0x36,0x09,0x48,0x85,0xc9,0x74,0x18})||
       !bytes(0x90934d0,{0x40,0x53,0x48,0x83,0xec,0x50})||
       !bytes(attendance::bindings::AttendanceUpdate,{0x40,0x55,0x56,0x41,0x57,0x48,0x8d,0xac,0x24,0x20,0xe9,0xff,0xff,0xb8,0xe0,0x17,0x00,0x00})||
       !bytes(attendance::bindings::PresentationUpdate,{0x40,0x53,0x48,0x83,0xec,0x40,0x8b,0x81,0xa0,0,0,0,0x48,0x8b,0xd9})||
       !bytes(0x5d01b70,{0x48,0x63,0xc2,0x48,0x8b,0x4c,0xc1,0x20,0xc5,0xfa,0x10,0x41,0x28,0xc3})||
       !bytes(0x5d01b80,{0x48,0x8b,0x41,0x28,0x8b,0x51,0x08}))return false;
    signatures=true;
    superSimBinding=bytes(attendance::bindings::SuperSimModeGetter,{0x48,0x8b,0x05,0xd1,0x98,0x17,0x0c,0x48,0x85,0xc0,0x74,0x0d,0x83,0x78,0x08,0x01,0x75,0x07,0xb1,0x01});
    if(!superSimBinding)log("simulation-unavailable","Super Sim binding differs; crowd changes will wait for ordinary selection and breaks.");
    pauseBinding=bytes(attendance::bindings::PauseCheck,{0x48,0x8b,0x05,0xa9,0x0c,0x9a,0x07,0x48,0x85,0xc0,0x74,0x08,0x80,0x38,0x00,0x74,0x03,0xb0,0x01,0xc3,0x32,0xc0,0xc3});
    replayBinding=bytes(attendance::bindings::ReplayTimeGetter,{0xc5,0xfb,0x10,0x41,0x40,0xc3})&&
        bytes(attendance::bindings::RecordingTimeGetter,{0xc5,0xfb,0x10,0x81,0x30,0x10,0,0,0xc3})&&
        bytes(attendance::bindings::ReplayActiveGetter,{0x0f,0xb6,0x41,0x38,0xc3})&&
        bytes(attendance::bindings::ReplayStartGetter,{0xc5,0xfb,0x10,0x41,0x48,0xc3});
    if(!replayBinding)log("replay-unavailable","Replay time bindings differ; historical crowd playback is inactive.");
    if(replayBinding){
        const auto& prefix=attendance::replayhook::Prefix;
        if(!bytes(attendance::bindings::ReplayUpdate,std::vector<unsigned char>(prefix.begin(),prefix.end())))replayBinding=false;
        else if(!makePatch(attendance::bindings::ReplayUpdate,reinterpret_cast<void*>(replayUpdate),reinterpret_cast<void**>(&originalReplayUpdate),17))return false;
        if(!replayBinding)log("replay-unavailable","Replay update callback differs; historical crowd playback is inactive.");
    }
    if(!pauseBinding)log("anticipation-inactive","Pause-state binding differs; new roars and temporary excitement will remain inactive.");
    if(pauseBinding&&replayBinding&&
       bytes(attendance::bindings::MuseLogicEvaluate,cleanEntry(attendance::bindings::MuseLogicEvaluate))&&
       bytes(attendance::bindings::MuseComparisonValue,cleanEntry(attendance::bindings::MuseComparisonValue))){
        if(!makePatch(attendance::bindings::MuseLogicEvaluate,reinterpret_cast<void*>(museLogicEvaluate),reinterpret_cast<void**>(&originalMuseLogic),15)||
           !makePatch(attendance::bindings::MuseComparisonValue,reinterpret_cast<void*>(museComparisonValue),reinterpret_cast<void**>(&originalMuseComparison),15))return false;
        replayPresentationInstalled=true;
    }else log("replay-unavailable","Extra post-play replays are unavailable on this layout; normal replay selection remains active.");
    // These getters independently verify every new play-data field. A changed
    // layout disables energy/events while retaining the original attendance.
    playBindingsAvailable=bytes(attendance::bindings::CurrentDownGetter,{0x48,0x8b,0x05,0x99,0xd0,0x65,0x09})&&
        bytes(attendance::bindings::CurrentDownGetter+0x15,{0x8b,0x80,0xd0,0x01,0x00,0x00})&&
        bytes(attendance::bindings::CurrentOffenseGetter,{0x48,0x8b,0x05,0x19,0xbc,0x65,0x09})&&
        bytes(attendance::bindings::CurrentOffenseGetter+0x15,{0x8b,0x80,0xc8,0x01,0x00,0x00})&&
        bytes(attendance::bindings::TimeoutGetter,{0x8b,0x41,0x4c,0x89,0x44,0x24,0x08,0x8b,0x41,0x50})&&
        bytes(attendance::bindings::FirstDownGetter,{0x48,0x8b,0x05,0x09,0xc3,0x65,0x09})&&
        bytes(attendance::bindings::FirstDownGetter+0x15,{0x48,0x81,0xc1,0x60,0x02,0x00,0x00})&&
        bytes(attendance::bindings::FirstDownGetter+0x2a,{0x8b,0x41,0x20,0xc1,0xe8,0x0a,0x24,0x01})&&
        bytes(attendance::bindings::FirstDownLine,{0x48,0x83,0xec,0x18,0x48,0x8b,0xd1})&&
        bytes(0x61fa5a5,{0xc4,0xe2,0x79,0x18,0x98,0x6c,0x01,0x00,0x00})&&
        bytes(0x2e905d0,{0x48,0x8b,0x05,0x09,0x1a,0x54,0x0c,0xc3})&&
        bytes(attendance::bindings::TurnoverGetter,{0x48,0x8b,0x81,0xd8,0x0c,0x00,0x00,0x8b,0x40,0x64,0xc3});
    findEntities();
    if(!makePatch(attendance::bindings::AttendanceUpdate,reinterpret_cast<void*>(attendanceUpdate),reinterpret_cast<void**>(&originalAttendanceUpdate),18)||
       !makePatch(attendance::bindings::PresentationUpdate,reinterpret_cast<void*>(presentationUpdate),reinterpret_cast<void**>(&originalPresentationUpdate),15)||
       !makePatch(0x64afda0,reinterpret_cast<void*>(visibility),reinterpret_cast<void**>(&originalSet),15)||
       !makePatch(0x5d024c0,reinterpret_cast<void*>(periodChanged),reinterpret_cast<void**>(&originalPeriod),18))return false;
    const std::vector<unsigned char> audioQueue={0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x89,0x7c,0x24,0x60};
    if(bytes(attendance::bindings::AudioUpdate,cleanEntry(attendance::bindings::AudioUpdate))&&bytes(attendance::bindings::CrowdQueue,audioQueue)){
        if(!makePatch(attendance::bindings::AudioUpdate,reinterpret_cast<void*>(audioUpdate),reinterpret_cast<void**>(&originalAudioUpdate),16))return false;
        crowdAudio.configure(image,originalAudioUpdate,audioWanted,audioNote);audioHookInstalled=true;
        if(swellBindings()){
            if(!makePatch(attendance::bindings::SwellSend,reinterpret_cast<void*>(crowdSend),reinterpret_cast<void**>(&originalCrowdSend),17))return false;
            auto api=attendance::sustained::Api::native(image);api.send=originalCrowdSend;
            sustainedAudio.configure(image,sustainedNote,&api);swellAvailable=true;
        }
        else log("anticipation-inactive","Sustained roar bindings differ; existing crowd audio remains available.");
        if(swellBindings()&&bytes(attendance::bindings::ReactionUpdate,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x74,0x24,0x20})&&
           bytes(attendance::bindings::ReactionReceive,{0x40,0x57,0x48,0x83,0xec,0x70})){
            positiveCheer.configure(image);
            if(!makePatch(attendance::bindings::ReactionUpdate,reinterpret_cast<void*>(reactionUpdate),reinterpret_cast<void**>(&originalReactionUpdate),15))return false;
            cheerAvailable=true;
        }else log("cheer-inactive","Native cheer bindings differ; celebration roars and volume remain available.");
        if(bytes(attendance::bindings::IntensityUpdate,cleanEntry(attendance::bindings::IntensityUpdate))&&
           bytes(attendance::bindings::IntensityPublish,{0x48,0x8b,0x09,0xe9,0x58,0x30,0,0})&&
           bytes(attendance::bindings::IntensityLock,{0x48,0xff,0x25,0x21,0x64,0x63,0x10})&&
           bytes(attendance::bindings::IntensityUnlock,{0x48,0xff,0x25,0xc9,0x55,0x63,0x10})){
            crowdIntensity.configure(image);
            if(!makePatch(attendance::bindings::IntensityUpdate,reinterpret_cast<void*>(intensityUpdate),reinterpret_cast<void**>(&originalIntensityUpdate),18))return false;
            intensityAvailable=true;
        }else log("intensity-inactive","Native excitement bindings differ; sustained roar playback and existing audio remain available.");
        if(bytes(attendance::bindings::MixerCreate,{0x4c,0x8b,0xdc,0x53,0x55,0x48,0x83,0xec,0x78})&&
           bytes(attendance::bindings::MixerRelease,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20})){
            chantAudio.configure(image,audioNote);crowdBoostAudio.configure(image,audioNote,nullptr,nullptr,nullptr,6);
            ambienceAudio.configure(image,audioNote,nullptr,nullptr,nullptr,4,nullptr,13);chantAvailable=true;
        }else log("chants-inactive","Native chant mixer layout differs; general crowd audio remains available");
    }else log("audio-inactive","Native audio layout differs; visible attendance remains available");
    if(!commitPatches())return false;
    enabled=true;return true;
}
// Read-only native context. The game uses these same options for its presentation data.
bool option(unsigned key,int width,int& value){
    A service=at<A>(image+0xe2dd3e8),vt=at<A>(service);
    if(vt!=image+0xb298a70||at<A>(vt+0x68)!=image+0x90934d0)return false;
    struct Variant {A vt,type,data;} v{image+0xb12f498,at<A>(image+0xfc88ab8),0};
    using Query=bool(__fastcall*)(A,unsigned,Variant*,A,A);
    if(!reinterpret_cast<Query>(image+0x90934d0)(service,key,&v,0,0)||!v.data)return false;
    if(width==1){unsigned char x=0;if(!read(v.data,&x,1))return false;value=x;}
    else {unsigned short x=0;if(!read(v.data,&x,2))return false;value=x;}
    return true;
}
bool context(attendance::State& s){
    A weather=at<A>(image+0xf4fe000);
    if(at<A>(weather)!=image+0xb323ed0)return false;
    bool overrideWeather=at<unsigned char>(weather+0x388)!=0;
    float precipitation=at<float>(weather+(overrideWeather?0x39c:0xf0));
    int temperature=at<int>(weather+(overrideWeather?0x38c:0xe8));
    if(!std::isfinite(precipitation)||precipitation<0||temperature<-150||temperature>160)return false;
    s.precipitation=precipitation>0?(temperature<=32?2:1):0;
    int rivalry=1023;
    if(!option(0x9e2b2900,1,s.bowl)||!option(0x9f2e6900,2,rivalry))return false;
    // Same read-only predicate used by native HFA suppression and the UI's
    // IsNeutralStadium query. It includes the home program's alternate stadium.
    using Neutral=bool(__fastcall*)();
    s.neutral=reinterpret_cast<Neutral>(image+attendance::bindings::NeutralGetter)();
    s.rivalry=rivalry!=1023;
    s.playoff=attendance::playoffBowl(s.bowl);
    int team0=at<int>(image+0xd9f3970),team1=at<int>(image+0xd9f3974);
    if(team0<0||team0>=2047||team1<0||team1>=2047)return false;
    s.team0=team0;s.team1=team1;
    // This native reader supplies HomeTeamRank/AwayTeamRank to the game UI.
    using Rank=int(__fastcall*)(A,int);
    auto rank=reinterpret_cast<Rank>(image+0xa0ad090);
    s.rank0=rank(0,team0);s.rank1=rank(0,team1);
    if(s.rank0 < -1||s.rank0>255||s.rank1 < -1||s.rank1>255)return false;
    return true;
}
bool guardedContext(attendance::State& s){__try{return context(s);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
void showContext(const attendance::Settings& c,const attendance::State& s){
    bool matchupChanged=!contextReady||shownRank0!=s.rank0||shownRank1!=s.rank1||shownTeam0!=s.team0||shownTeam1!=s.team1||shownRivalry!=s.rivalry||shownNeutral!=s.neutral||shownPlayoff!=s.playoff||shownPrecipitation!=s.precipitation;
    contextReady=true;
    if(matchupChanged){
        auto rankText=[](int rank){return rank>0?std::to_string(rank):std::string("unknown/unranked, raw ")+std::to_string(rank);};
        log("matchup","Matchup: home team ID "+std::to_string(s.team0)+" (rank "+rankText(s.rank0)+"); away team ID "+std::to_string(s.team1)+" (rank "+rankText(s.rank1)+"); rivalry "+(s.rivalry?"yes":"no")+"; neutral venue "+(s.neutral?"yes":"no")+"; postseason "+(s.playoff?"yes":"no")+"; weather "+(s.precipitation==1?"rain":s.precipitation==2?"snow":"dry")+".");
    }
    shownProfile=static_cast<int>(attendance::profileFor(c,s));shownRank0=s.rank0;shownRank1=s.rank1;
    shownRivalry=s.rivalry;shownPlayoff=s.playoff;shownBowl=s.bowl;shownPrecipitation=s.precipitation;
    shownNeutral=s.neutral;shownTeam0=s.team0;shownTeam1=s.team1;shownFcs=attendance::arrivals::fcsOpponent(s);
}
void readSettings(){
    auto c=std::make_shared<attendance::Settings>();auto path=pluginsFolder/L"DynamicGamedayAtmosphere.ini";
    auto number=[&](const wchar_t* section,const wchar_t* key,int fallback,int low,int high){
        return (std::clamp)(static_cast<int>(GetPrivateProfileIntW(section,key,fallback,path.c_str())),low,high);
    };
    auto percentage=[&](const wchar_t* section,const wchar_t* key,float fallback,float maximum){
        wchar_t raw[128]{},*end=nullptr;GetPrivateProfileStringW(section,key,L"",raw,128,path.c_str());
        float value=std::wcstof(raw,&end);if(end==raw||!std::isfinite(value))return fallback;
        while(std::iswspace(*end))++end;if(*end&&*end!=';'&&*end!='#')return fallback;
        return (std::clamp)(value,0.0f,maximum);
    };
    c->enabled=GetPrivateProfileIntW(L"Attendance",L"Enabled",1,path.c_str())!=0;
    extraReplays=number(L"Replays",L"Enabled",1,0,1)!=0;
    routineInsets=number(L"Replays",L"InsetAfterRoutinePlays",1,0,1)!=0;
    bigPlayReplays=number(L"Replays",L"FullScreenBigPlays",1,0,1)!=0;
    defensiveReplays=number(L"Replays",L"DefensiveHighlights",1,0,1)!=0;
    replayMinimumYards=number(L"Replays",L"FullScreenMinimumYards",15,10,60);
    audioSetting=GetPrivateProfileIntW(L"Audio",L"Enabled",1,path.c_str())!=0;
    audioReductionPercent=number(L"Audio",L"MaximumReductionPercent",20,0,100);
    audioReductionStart=number(L"Audio",L"ReductionStartPercent",80,1,100);
    chantStrength=(std::clamp)(static_cast<int>(GetPrivateProfileIntW(L"Audio",L"ChantStrength",100,path.c_str())),0,100);
    chantReduction=percentage(L"Audio",L"ChantReductionPercent",50,100);
    arrivalEnabled=number(L"Arrivals",L"Enabled",1,0,1);arrivalStart=number(L"Arrivals",L"StartPercent",70,10,100);
    arrivalLateStart=number(L"Arrivals",L"LateSchoolStartPercent",50,10,100);
    arrivalFinish=number(L"Arrivals",L"FinishPercentOfQuarter",25,10,100);
    arrivalLateFinish=number(L"Arrivals",L"LateSchoolFinishPercentOfQuarter",75,10,100);
    arrivalModerateStart=number(L"Arrivals",L"ModerateStartPercent",60,10,100);
    arrivalModerateFinish=number(L"Arrivals",L"ModerateFinishPercentOfQuarter",50,10,100);
    halftimeExtra=number(L"Arrivals",L"HalftimeSeatBreakStrength",100,0,200);
    arrivalBigBonus=number(L"Arrivals",L"BigGameBonus",20,0,20);
    arrivalWeatherPenalty=number(L"Arrivals",L"BadWeatherPenalty",10,0,30);
    arrivalPlayoffStart=number(L"Arrivals",L"PlayoffStartPercent",65,10,100);
    arrivalChampionshipStart=number(L"Arrivals",L"ChampionshipStartPercent",75,10,100);
    arrivalFcsPenalty=number(L"Arrivals",L"FcsOpponentPenalty",10,0,30);

    logLimitMB=number(L"Logging",L"MaxFileMB",2,1,16);debugLog=number(L"Logging",L"Debug",0,0,1)!=0;
    c->comebackDeficit=number(L"Retention",L"ComebackOriginalDeficit",21,14,100);
    c->comebackRecovered=number(L"Retention",L"ComebackPointsRecovered",14,7,100);
    c->comebackDeteriorationSeconds=number(L"Retention",L"ComebackDeteriorationSeconds",60,0,600);
    c->warningEnabled=number(L"Retention",L"TwoMinuteWarning",1,0,1);
    c->quarterCap=number(L"Retention",L"MaximumDeparturePerQuarter",45,1,50);
    c->transitionCap=number(L"Retention",L"MaximumDeparturePerTransition",4,1,20);
    c->formulaStrength=number(L"Retention",L"FormulaStrength",100,0,200);
    auto energy=std::make_shared<attendance::energy::Settings>();
    // Legacy event context is retained only for attendance protection; it no longer drives audio.
    auto core=std::make_shared<attendance::core::Settings>();
    core->enabled=number(L"Atmosphere",L"Enabled",1,0,1)!=0;
    core->opening=number(L"Atmosphere",L"OpeningRoar",1,0,1)!=0;
    core->overtime=number(L"Atmosphere",L"OvertimeRoar",1,0,1)!=0;
    core->eruptions=number(L"Atmosphere",L"WinningEruptions",1,0,1)!=0;
    core->maximumPercent=percentage(L"Atmosphere",L"MaximumBoostPercent",attendance::core::NormalMaximumPercent,attendance::core::NormalMaximumPercent);
    core->eruptionPercent=percentage(L"Atmosphere",L"EruptionBoostPercent",attendance::core::EruptionMaximumPercent,attendance::core::EruptionMaximumPercent);
    std::shared_ptr<const attendance::core::Settings> nextCore=core;std::atomic_store(&coreSettings,nextCore);
    energy->lateScoreDepartures=number(L"Retention",L"LateScoreExtra",5,0,25);
    std::shared_ptr<const attendance::energy::Settings> nextEnergy=energy;std::atomic_store(&retentionEventSettings,nextEnergy);
    wchar_t profile[64]{};GetPrivateProfileStringW(L"Attendance",L"Profile",L"Auto",profile,64,path.c_str());
    if(!_wcsicmp(profile,L"Default"))c->profile=attendance::Profile::Default;
    else if(!_wcsicmp(profile,L"Reduced"))c->profile=attendance::Profile::Reduced;
    else if(!_wcsicmp(profile,L"BadWeather"))c->profile=attendance::Profile::BadWeather;
    std::shared_ptr<const attendance::Settings> next=c;std::atomic_store(&settings,next);
}
bool playData(A match,attendance::energy::Play& p){
    if(!playBindingsAvailable)return false;
    if(!read(match+0x1d0,&p.down,4)||!read(match+0x1c8,&p.offense,4))return false;
    if(p.down < -2||p.down>13||p.offense<0||p.offense>1)return false;
    A wrapper=at<A>(image+attendance::bindings::MatchWrapper),rules=at<A>(wrapper+0x18);
    if(at<A>(rules)==image+attendance::bindings::RulesVtable){
        int count=-1;if(read(rules+0x4c,&count,4)&&count>=0&&count<=3)p.homeTimeouts=count;
    }
    unsigned flags=0;if(read(match+0x280,&flags,4))p.firstDown=(flags&(1u<<10))!=0;
    using FirstLine=float(__fastcall*)(A);
    float line=reinterpret_cast<FirstLine>(image+attendance::bindings::FirstDownLine)(match),spot=0;
    if(read(match+0x16c,&spot,4)&&std::isfinite(line)&&std::isfinite(spot)){
        float distance=std::fabs(line-spot);if(distance<=100)p.toFirst=distance;
        if(std::fabs(spot)<=50.01f&&std::fabs(line)<=50.01f){
            p.fieldSpot=spot;
            if(distance>0.01f)p.attackDirection=line>spot?1:-1;
            // Before the snap the verified first-down line identifies the attacking
            // end zone. This does not depend on which team the user controls.
            if(p.game.phase==1&&p.attackDirection)p.toGoal=std::fabs(p.attackDirection*50.0f-spot);
        }
        // A first-down line at an end-zone boundary is a verified goal-to-go
        // situation. This remains available without a selected-player context.
        if(std::fabs(std::fabs(line)-50)<0.01f&&distance<=10)p.toGoal=distance;
    }
    // Optional presentation field-position source. Do not use its default
    // value when the game has no active selected-player context.
    int index=at<int>(image+attendance::bindings::SelectedPlayer);
    if(index>=0&&index<2){
        A actor=at<A>(image+attendance::bindings::PlayerContexts+static_cast<A>(index)*8),field=at<A>(actor+0xa8);
        float goal=0,current=0;
        if(field&&read(field+0xc4,&goal,4)&&read(match+0x30,&current,4)&&std::isfinite(goal)&&std::isfinite(current)&&std::fabs(std::fabs(goal)-50)<0.01f){
            // Verify the first-down line points toward this end zone. This
            // avoids treating a team backed up at its own goal as attacking it.
            if((line-spot)*(goal-spot)>0&&std::fabs(line-spot)>0.01f){
                float distance=std::fabs(goal-spot);if(distance<=100)p.toGoal=distance;
            }
        }
    }
    A tracker=at<A>(image+attendance::bindings::PlayStats),record=at<A>(tracker+0xcd8);
    unsigned char result[48]{};
    if(record&&read(record+0x58,result,sizeof(result))){
        uint64_t stamp=1469598103934665603ull;
        for(unsigned char b:result){stamp^=b;stamp*=1099511628211ull;}
        // Include native result identity so equally shaped OT plays remain
        // distinguishable while the untimed game clock stays at zero.
        stamp^=static_cast<uint64_t>(record);stamp*=1099511628211ull;
        p.resultStamp=stamp;p.interception=at<int>(record+0x64)==2;
    }
    liveDown=p.down;liveOffense=p.offense;liveHomeTimeouts=p.homeTimeouts;return true;
}
void atmosphereTick(){
    auto now=GetTickCount64();if(now-lastAtmosphereTick<50)return;lastAtmosphereTick=now;
    auto c=std::atomic_load(&retentionEventSettings);
    auto gap=[&](){
        std::shared_ptr<const attendance::sustained::Intent> quiet=std::make_shared<attendance::sustained::Intent>();std::atomic_store(&sustainedIntent,quiet);
        auto wanted=audioWanted();std::lock_guard<std::mutex> guard(atmosphereMutex);
        crowdBoost=audioContinuity.gap(wanted.match,now/1000.0,wanted.active&&mode==0&&std::atomic_load(&coreSettings)->enabled,attendance::core::normalLimit(*std::atomic_load(&coreSettings)));
        shownAudioLimit=attendance::core::normalLimit(*std::atomic_load(&coreSettings));
        if(crowdBoost<=0)shownAmbienceLift=shownOpeningWeight=0;
    };
    attendance::energy::Play p;A match=0;
    if(!gameState(p.game,match)||!contextReady){gap();return;}
    if(replaySuspendsAtmosphere){gap();return;}
    if(pauseBinding&&!nativeUnpaused()){
        // The postgame stats/highlights menu retains phase 7 and pauses the
        // native world. End the carry once so highlight playback cannot restart it.
        if(p.game.phase==7){
            bool ended=false;{std::lock_guard<std::mutex> guard(atmosphereMutex);ended=coreAtmosphere.finishPostgame();}
            shownEruption=shownPostgameCarry=false;shownDecisiveUntil=0;
            if(ended)log("roar","Postgame menu opened; winning celebration roar released.");
        }
        gap();return;
    }
    // Matchup atmosphere uses the verified basic game context even if optional
    // play details are unavailable. Those details only gate specific events.
    playData(match,p);
    p.game.rank0=shownRank0;p.game.rank1=shownRank1;p.game.rivalry=shownRivalry;p.game.playoff=shownPlayoff;
    p.game.neutral=shownNeutral;p.game.team0=shownTeam0;p.game.team1=shownTeam1;p.game.bowl=shownBowl;
    if(p.game.phase==3){std::lock_guard<std::mutex> guard(historyMutex);matchLifecycle.live();}
    std::lock_guard<std::mutex> guard(atmosphereMutex);
    if(atmosphereMatch!=match){playTracker.reset();coreAtmosphere.reset();audioContinuity.reset();pendingLateExits=0;atmosphereMatch=match;}
    auto result=playTracker.observe(*c,p);
    if(!result.accepted){
        std::shared_ptr<const attendance::sustained::Intent> quiet=std::make_shared<attendance::sustained::Intent>();std::atomic_store(&sustainedIntent,quiet);
        shownAudioLimit=attendance::core::normalLimit(*std::atomic_load(&coreSettings));auto wanted=audioWanted();
        crowdBoost=audioContinuity.gap(match,now/1000.0,wanted.active&&mode==0&&std::atomic_load(&coreSettings)->enabled,attendance::core::normalLimit(*std::atomic_load(&coreSettings)));
        if(crowdBoost<=0)shownAmbienceLift=shownOpeningWeight=0;return;
    }
    double time=now/1000.0;
    auto audio=std::atomic_load(&coreSettings);
    uint64_t previousCelebration=coreAtmosphere.celebration;
    coreAtmosphere.react(*audio,result,time,p.game);
    if(result.lateDeparture){
        pendingLateExits=(std::min)(100,pendingLateExits+c->lateScoreDepartures);
        log("late-score","Late opponent score: home trails by "+std::to_string(p.game.awayScore-p.game.homeScore)+", with "+std::to_string(p.homeTimeouts)+(p.homeTimeouts==1?" timeout":" timeouts")+"; extra departures queued for the break before the next play.");
    }
    float remaining=(100-(std::max)(activeLoss.load(),arrivalMissing.load()))*0.01f;
    float permanent=(100-activeLoss.load())*0.01f;
    float previousOpening=coreAtmosphere.openingWeight;bool wasPostgameHeld=coreAtmosphere.postgameHeld;
    coreAtmosphere.update(*audio,p,time,remaining,permanent);
    if(!wasPostgameHeld&&coreAtmosphere.postgameHeld)log("roar","Winning eruption carried into postgame; roar held until the stats/highlights menu.");
    if(previousCelebration!=coreAtmosphere.celebration){
        int seconds=static_cast<int>(p.game.seconds);char stamp[32]{};
        if(p.game.phase==7)sprintf_s(stamp,"Final");else sprintf_s(stamp,"Q%d %d:%02d",p.game.quarter,seconds/60,seconds%60);
        log("eruption",std::string(attendance::energy::name(coreAtmosphere.event))+" ("+(coreAtmosphere.winner==1?"away":"home")+") at "+stamp+"; 45-second celebration, sustained roar and native cheer; volume boost up to +"+std::to_string(static_cast<int>(std::lround(attendance::volume::boostPercent(attendance::core::eruptionLimit(*audio)))))+"%.");
    }
    if(previousOpening<=0&&coreAtmosphere.openingWeight>0)log("opening","Big-game opening atmosphere started.");
    if(previousOpening>0&&coreAtmosphere.openingWeight<=0)log("opening","Opening atmosphere settled into the normal crowd mix.");
    shownAmbienceLift=coreAtmosphere.beds;shownOpeningWeight=coreAtmosphere.openingWeight;shownAudioLimit=coreAtmosphere.limit;
    shownDecisiveUntil=coreAtmosphere.eruptionLease(now);
    shownEruption=coreAtmosphere.erupting;
    shownPostgameCarry=!coreAtmosphere.postgameMenu&&(coreAtmosphere.postgameHeld||(coreAtmosphere.postgameAt>=0&&time-coreAtmosphere.postgameAt<10));
    audioEmphasis=1;
    bool live=p.game.phase==1||p.game.phase==3||p.game.phase==4||p.game.phase==5||p.game.phase==6||p.game.phase==7;
    bool active=mode==0&&remaining>0&&audio->enabled;
    auto wanted=coreAtmosphere.intent(*audio,p,match,audioEpoch.load(),now,active&&enabled&&audioSetting);
    std::shared_ptr<const attendance::sustained::Intent> next=std::make_shared<attendance::sustained::Intent>(wanted);std::atomic_store(&sustainedIntent,next);
    if(live&&active)crowdBoost=audioContinuity.record(match,time,coreAtmosphere.parent,coreAtmosphere.limit);
    else crowdBoost=audioContinuity.gap(match,time,active,coreAtmosphere.limit);

}
bool guardedAtmosphereTick(){__try {atmosphereTick();return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
void temporarySeats(attendance::State s,int permanent){
    int missing=0,bathroom=0,arriving=0;bool was=false,now=false;
    {std::lock_guard<std::mutex> guard(historyMutex);
        was=halftimeModel.active;
        // Authoritative quarter changes can report QuarterEnd rather than Preplay.
        s.phase=5;bathroom=halftimeModel.observe(s,permanent,halftimeExtra.load());
        now=halftimeModel.active;arriving=arrivalModel.missing;
        missing=departureModel.paceTemporary(permanent,(std::max)(arriving,bathroom));
        if(s.quarter==3)halftimeModel.lastAbsent=(std::min)(halftimeModel.lastAbsent,(std::max)(0,missing-permanent));
    }
    shownArriving=s.quarter<3&&bathroom==0?missing:arriving?missing:0;
    shownHalftime=bathroom>0||s.quarter==3?(std::max)(0,missing-permanent):0;
    if(was!=now)log("halftime",now?(s.quarter==2?"Fans are leaving their seats early for halftime; these temporary seats can fill again after the break.":"Halftime seat break active; temporarily absent fans will return during Q3."):"Halftime seat break ended; temporarily absent fans may return. Permanent departures stay empty.");
    if(arrivalMissing.exchange(missing)!=missing)arrivalReconcilePending=true;
}
void checkpoint(const attendance::Settings& c,const attendance::State& s,bool warning){
    attendance::departures::Result result;std::string why;
    {std::lock_guard<std::mutex> guard(historyMutex);
        departureModel.observe(c,s,gameHistory,mode==0,true);
        result=departureModel.settle(s,static_cast<int>(currentLoss),0,c.quarterCap);
        why=attendance::departures::reason(c,s,gameHistory,result.catchup);
    }
    int before=100-static_cast<int>(currentLoss),after=100-result.loss;
    int margin=s.homeScore-s.awayScore;
    std::string text="Q"+std::to_string(s.quarter)+(warning?" two-minute checkpoint: ":" start: ");
    text+=margin==0?"score tied":std::string(s.neutral?"margin ":"home ")+(s.neutral?"":margin>0?"leads by ":"trails by ")+std::to_string(std::abs(margin));
    text+="; crowd ";
    if(after==before)text+="held at "+std::to_string(after)+"%";
    else text+="reduced from "+std::to_string(before)+"% to "+std::to_string(after)+"%";
    log(warning?"two-minute":"quarter-break",text+" ("+why+").");
    temporarySeats(s,result.loss);
    bool reconcile=arrivalReconcilePending.exchange(false);
    if(result.loss!=lastApplied||reconcile||visibilityGeneration!=entityGeneration.load()){
        removalLimit=static_cast<unsigned int>(result.loss*65536u/100u);activeLoss=result.loss;
        if(!guardedApply(result.loss>0||reconcile,reconcile)){enabled=false;log("fault","Crowd update failed; further atmosphere changes stopped.");return;}
        lastApplied=result.loss;
        visibilityGeneration=entityGeneration.load();
    }
    currentLoss=highestTarget=static_cast<float>(result.loss);shownCurrent=currentLoss;shownTarget=highestTarget;
}
attendance::arrivals::Settings arrivalSettings(){
    return {arrivalEnabled.load(),arrivalStart.load(),arrivalLateStart.load(),arrivalFinish.load(),arrivalLateFinish.load(),arrivalBigBonus.load(),arrivalWeatherPenalty.load(),arrivalPlayoffStart.load(),arrivalChampionshipStart.load(),arrivalFcsPenalty.load(),arrivalModerateStart.load(),arrivalModerateFinish.load()};
}
void beginAttendanceHistory(attendance::State& s,bool midgame){
    attendance::timeline::Point saved;bool matched=false;
    if(midgame){std::lock_guard<std::mutex> guard(timelineMutex);matched=attendanceArchive.resume(s,saved);}
    FILETIME stamp{};GetSystemTimeAsFileTime(&stamp);uint64_t id=(uint64_t(stamp.dwHighDateTime)<<32)|stamp.dwLowDateTime;
    s.variationSeed=matched?saved.state.variationSeed:static_cast<unsigned>(id^(id>>32));
    if(attendanceSeed.exchange(s.variationSeed)!=s.variationSeed){
        std::lock_guard<std::mutex> guard(crowdMutex);for(auto& item:startingCrowds)item.second.selection={};
        seatPatternPending=!startingCrowds.empty();visibilityRetry=true;
    }
    if(midgame&&mode==0){
        auto c=std::atomic_load(&settings);auto seed=attendance::timeline::estimate(*c,arrivalSettings(),s,matched?&saved:nullptr,halftimeExtra.load());
        {std::lock_guard<std::mutex> guard(historyMutex);gameHistory=seed.history;departureModel=seed.departures;arrivalModel=seed.arrivals;halftimeModel=seed.halftime;}
        currentLoss=highestTarget=static_cast<float>(seed.loss);activeLoss=seed.loss;arrivalMissing=seed.temporary;
        shownCurrent=currentLoss;shownTarget=highestTarget;shownArriving=seed.arrivals.missing;
        shownHalftime=seed.halftime.active?(std::max)(0,seed.temporary-seed.loss):0;
        shownResumeSource=matched?2:1;exactVisibility=true;lastApplied=-1;visibilityRetry=true;
        log("resumed-game",std::string(matched?"Restored recorded attendance":"Estimated attendance; no matching crowd history")+": Q"+std::to_string(s.quarter)+", home "+std::to_string(s.homeScore)+" - away "+std::to_string(s.awayScore)+"; target "+std::to_string(100-(std::max)(seed.loss,seed.temporary))+"% of this game's original crowd. Future departures use this new scenario.");
    }
    {std::lock_guard<std::mutex> guard(timelineMutex);attendanceArchive.begin(id,s);}
    bootstrapPending=false;
}
bool ordinaryOpeningAcknowledgement(A match,const attendance::State& s){
    if(!contextReady||match!=lastMatch||s.quarter!=1||s.homeScore||s.awayScore||s.seconds<s.quarterSeconds*0.95f||
       (s.phase!=6&&s.phase!=1&&s.phase!=5)||at<int>(image+0xd9f3970)!=shownTeam0||at<int>(image+0xd9f3974)!=shownTeam1)return false;
    std::lock_guard<std::mutex> guard(historyMutex);return matchLifecycle.keepAtKickoff(match);
}
// Runs on presentation and native replay callbacks, including paused viewers.
// It changes visibility only; the live departure/history/audio models stay put.
bool replayPresentation(){
    auto replay=replaySnapshot();
    attendance::State s;A match=0;
    if(replay.active||replayWasActive||replayRestorePending){
        // Never restore or replay an old scene before the current match has
        // initialized. Let decision() see a world change or a resumed game.
        bool verifiedGame=gameState(s,match);
        bool currentSession=true;
        if(verifiedGame&&match==lastMatch&&contextReady){
            {std::lock_guard<std::mutex> guard(historyMutex);
                currentSession=!matchLifecycle.needsReset(match,s.phase==6)&&
                    !attendance::timeline::recordingRestarted(replay.valid,replay.recording,lastRecordingTime,s,departureModel.previous);
            }
            {std::lock_guard<std::mutex> guard(timelineMutex);currentSession=currentSession&&attendanceArchive.active!=0;}
            int team0=at<int>(image+0xd9f3970),team1=at<int>(image+0xd9f3974);
            if(team0>0&&team0<2047&&team1>0&&team1<2047&&(team0!=shownTeam0||team1!=shownTeam1))currentSession=false;
            auto signal=std::atomic_load(&resumeSignal);
            if(resumeTracker.pending(*signal,s,GetTickCount64(),ordinaryOpeningAcknowledgement(match,s)))currentSession=false;
        }
        if(!attendance::timeline::ownsReplayCrowd(verifiedGame,match,lastMatch,contextReady,currentSession)){
            replayLoss=-1;replaySuspendsAtmosphere=false;replayWasActive=false;replayRestorePending=false;
            return false;
        }
    }
    // Hold the current picture through a transient replay-manager rebuild.
    // A verified new world above still clears ownership and initializes normally.
    if(replayBinding&&!replay.valid)return replayWasActive||replayRestorePending;
    bool paused=pauseBinding&&!nativeUnpaused(),finalMenu=false;
    if(replay.active&&!paused){
        if(s.phase==7){std::lock_guard<std::mutex> guard(atmosphereMutex);finalMenu=coreAtmosphere.postgameMenu;}
    }
    auto policy=attendance::timeline::replayPolicy(replay.active,paused,finalMenu);
    replaySuspendsAtmosphere=policy.suspendAtmosphere;
    if(policy.historical){
        attendance::timeline::Point point;bool found=false;
        {std::lock_guard<std::mutex> guard(timelineMutex);found=attendanceArchive.replay(replay.playback,point);}
        int loss=found?point.total():(std::max)(activeLoss.load(),arrivalMissing.load());
        bool changedTarget=replayLoss.exchange(loss)!=loss;
        if(!replayWasActive)log("replay",found?std::string(policy.suspendAtmosphere?"Replay":"Automatic replay")+": showing the recorded crowd; live attendance is preserved.":"No earlier crowd record; using this scenario's initialized attendance.");
        replayWasActive=true;
        if(changedTarget||visibilityRetry||visibilityGeneration!=entityGeneration.load()){
            if(!guardedApply(true,true)){enabled=false;log("fault","Replay crowd update failed; automatic updates stopped.");return true;}
            visibilityGeneration=entityGeneration.load();
        }
        return true;
    }
    if(replayWasActive){replayWasActive=false;replayLoss=-1;replayRestorePending=true;log("replay","Replay ended; restoring the live game's crowd.");}
    if(replayRestorePending){
        exactVisibility=true;
        if(!guardedApply(true,true)){enabled=false;log("fault","Live crowd restoration failed; automatic updates stopped.");return false;}
        if(!visibilityRetry){exactVisibility=false;replayRestorePending=false;}
    }
    return false;
}
bool guardedReplayPresentation(){__try{return replayPresentation();}__except(EXCEPTION_EXECUTE_HANDLER){replayLoss=-1;replaySuspendsAtmosphere=false;replayWasActive=false;replayRestorePending=true;return false;}}
bool guardedSimulationDecision(){
    __try {
        int phase=at<int>(at<A>(image+attendance::bindings::GameStates)+0xa0);
        if(phase!=0&&phase!=5)return true;
        if(!nativeUnpaused())return true;
        attendance::State s;A match=0;
        return !gameState(s,match)||!offscreenSimulation(s)||guardedDecision();
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void __fastcall replayUpdate(A entity){
    originalReplayUpdate(entity);++replayCallbacks;
    if(!enabled||at<A>(entity)!=image+attendance::bindings::ReplayEntityVtable)return;
    std::unique_lock<std::mutex> frame(presentationMutex,std::try_to_lock);if(!frame.owns_lock())return;
    // This callback also runs when gameplay/post-update is suspended. Broadcast
    // clips keep audio intents fresh without feeding replay state to departures.
    bool historical=guardedReplayPresentation();
    if(!historical){
        // Normal/Fast Super Sim suspends the main presentation callback, but
        // this native replay entity still updates. Service only a verified
        // offscreen simulation, on this same serialized native update thread.
        if(!guardedSimulationDecision()){
            enabled=false;log("fault","Super Sim crowd update failed; automatic updates stopped.");
        }
    }
    if(historical&&!replaySuspendsAtmosphere&&!guardedAtmosphereTick()){
        playBindingsAvailable=false;crowdBoost=0;log("audio-inactive","Presentation audio update unavailable; attendance history remains active.");
    }
}
void recordAttendance(){
    static uint64_t last=0;static int loss=-1,missing=-1;auto now=GetTickCount64();
    if(now-last<250&&loss==activeLoss.load()&&missing==arrivalMissing.load())return;
    attendance::State s;A match=0;if(mode!=0||!std::atomic_load(&settings)->enabled||!contextReady||replayWasActive||!gameState(s,match)||match!=lastMatch)return;
    // Offscreen sim has no new filmed plays. Its frozen recording timestamp
    // must not overwrite the crowd attached to the last real replay clip.
    if(offscreenSimulation(s))return;
    if(s.phase!=6&&!nativeUnpaused())return;
    s.team0=shownTeam0;s.team1=shownTeam1;s.rank0=shownRank0;s.rank1=shownRank1;s.rivalry=shownRivalry;s.playoff=shownPlayoff;s.bowl=shownBowl;s.neutral=shownNeutral;s.precipitation=shownPrecipitation;
    s.variationSeed=attendanceSeed;
    auto replay=replaySnapshot();if(replay.active)return;
    if(simulationRecording>=0&&replay.valid&&replay.recording<=simulationRecording)return;
    simulationRecording=-1;
    attendance::timeline::Point p;
    if(!attendance::timeline::recordingTime(replayBinding,replay.valid,replay.recording,attendance::elapsedGameTime(s),p.time))return;
    p.state=s;p.loss=activeLoss;p.temporary=arrivalMissing;
    {std::lock_guard<std::mutex> guard(historyMutex);p.history=gameHistory;p.periodStart=(std::clamp)(departureModel.periodStartLoss,0,p.loss);}
    {std::lock_guard<std::mutex> guard(timelineMutex);attendanceArchive.record(p);}
    if(replay.valid)lastRecordingTime=replay.recording;
    last=now;loss=p.loss;missing=p.temporary;
}
bool guardedRecordAttendance(){__try{recordAttendance();return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
bool carryPreparedOpening(A match,const attendance::State& s){
    int home=at<int>(image+0xd9f3970),away=at<int>(image+0xd9f3974);
    bool same=contextReady&&home>0&&away>0&&home==shownTeam0&&away==shownTeam1;
    {std::lock_guard<std::mutex> guard(historyMutex);if(!matchLifecycle.carryOpening(match,s,same))return false;}
    A previous=lastMatch;lastMatch=match;audioMatch=match;
    {std::lock_guard<std::mutex> guard(atmosphereMutex);if(atmosphereMatch==previous)atmosphereMatch=match;}
    return true;
}
void decision(){
    const auto now=GetTickCount64();const int pending=request.load();
    attendance::State s;A match=0;if(!gameState(s,match)){
        crowdChangeWindow.reset();
        observeWorldExit();
        lastFrame=now;return;
    }
    if(s.phase==7){
        // A postgame cutscene can stream a fresh crowd renderer. Rebuild its
        // selection and reapply the final picture, without advancing rules or
        // inventing a new session from the final scoreboard.
        shownSuperSim=false;bool ready=postgameRendererRetry.exchange(false);
        if(!ready&&now-lastDecision<500)return;
        lastDecision=now;bool owned=match==lastMatch&&contextReady&&mode==0&&std::atomic_load(&settings)->enabled;
        {std::lock_guard<std::mutex> guard(historyMutex);owned=owned&&!matchLifecycle.needsReset(match,false);}
        if(owned){
            shownQuarter=s.quarter;shownPhase=s.phase;shownSeconds=s.seconds;shownScore0=s.homeScore;shownScore1=s.awayScore;
            if(visibilityRetry||seatPatternPending||visibilityGeneration!=entityGeneration.load()){
                if(!guardedApply(activeLoss>0||arrivalMissing>0,true)){enabled=false;log("fault","Postgame crowd renderer could not be refreshed safely.");return;}
                visibilityGeneration=entityGeneration.load();
            }
        }
        lastFrame=now;return;
    }
    const bool sim=offscreenSimulation(s);const auto visualState=s;
    crowdChangeWindow.observe(match,s,sim);
    // Track short phase transitions before throttling the heavier work.
    bool pregame=s.phase==6;
    bool rendererReady=pregame&&pregameRendererRetry.exchange(false);
    if(!pending&&!rendererReady&&now-lastDecision<500)return;
    if(!pregame&&s.phase!=1&&s.phase!=5&&!sim){lastFrame=now;return;}
    // Normalize only our calculation snapshot. Never change a native phase,
    // clock, replay selector or Super Sim setting.
    if(sim)s.phase=5;
    lastDecision=now;shownQuarter=s.quarter;shownSeconds=s.seconds;shownScore0=s.homeScore;shownScore1=s.awayScore;shownPhase=s.phase;
    const auto generation=entityGeneration.load();
    bool newMatch=false;
    auto signal=std::atomic_load(&resumeSignal);bool openingAcknowledgement=ordinaryOpeningAcknowledgement(match,s);int previousResumeGame=resumeTracker.game;
    bool nativeResume=resumeTracker.observe(*signal,s,now,openingAcknowledgement);
    if(openingAcknowledgement&&!nativeResume&&previousResumeGame&&resumeTracker.game!=previousResumeGame)
        log("new-game","Midgame Save confirmed the new game; keeping the prepared opening crowd.");
    int team0=at<int>(image+0xd9f3970),team1=at<int>(image+0xd9f3974);
    bool changedTeams=contextReady&&team0>0&&team0<2047&&team1>0&&team1<2047&&(team0!=shownTeam0||team1!=shownTeam1);
    auto replay=replaySnapshot();
    bool restartedRecording=attendance::timeline::recordingRestarted(replay.valid,replay.recording,lastRecordingTime,s,departureModel.previous);
    if(!pregame&&!nativeResume&&!changedTeams&&!restartedRecording)carryPreparedOpening(match,s);
    {std::lock_guard<std::mutex> guard(historyMutex);newMatch=matchLifecycle.needsReset(match,pregame);}
    newMatch=newMatch||nativeResume||changedTeams||restartedRecording;
    bool newSession=!lastMatch||newMatch||generation!=lastGeneration;
    if(newSession){
        bool first=!lastMatch||newMatch;
        if(newMatch){
            // Remove our old masks before taking the new game's baseline when
            // Midgame Save reuses the resident crowd renderer.
            replayLoss=-1;
            if(!guardedApply(false,true)){enabled=false;log("fault","Previous crowd could not be released safely.");return;}
            resetMatch();crowdChangeWindow.observe(match,visualState,sim);
        }
        bootstrapPending=bootstrapPending||first;
        lastMatch=match;lastGeneration=generation;
        audioMatch=match;
        lastApplied=static_cast<int>(currentLoss);observed.clear();lastFrame=now;
        attendance::State detail=s;if(guardedContext(detail)){showContext(*std::atomic_load(&settings),detail);}else log("context-unavailable","Waiting for valid matchup data");
        refreshEntities=true;log("session","quarter="+std::to_string(s.quarter)+" clock="+std::to_string(s.seconds)+" home:away="+std::to_string(s.homeScore)+":"+std::to_string(s.awayScore));
    }
    {std::lock_guard<std::mutex> guard(historyMutex);matchLifecycle.join(match,pregame);}
    if(!contextReady){attendance::State detail=s;if(guardedContext(detail))showContext(*std::atomic_load(&settings),detail);}
    // School arrival curves and postseason floors require verified venue/team context.
    if(!contextReady)return;
    s.rank0=shownRank0;s.rank1=shownRank1;s.rivalry=shownRivalry;s.playoff=shownPlayoff;s.precipitation=shownPrecipitation;
    s.neutral=shownNeutral;s.team0=shownTeam0;s.team1=shownTeam1;s.bowl=shownBowl;
    {std::lock_guard<std::mutex> guard(timelineMutex);if(!attendanceArchive.active)bootstrapPending=true;}
    if(bootstrapPending)beginAttendanceHistory(s,!pregame&&((nativeResume&&signal->applied)||s.quarter>1||s.homeScore||s.awayScore||s.seconds<s.quarterSeconds*0.95f));
    s.variationSeed=attendanceSeed;
    attendance::energy::Play detail;detail.game=s;
    if(!sim&&!pregame&&playData(match,detail)){s.offense=detail.offense;s.homeTimeouts=detail.homeTimeouts;s.toGoal=detail.toGoal;}
    const auto liveSettings=std::atomic_load(&settings);bool warning=false,accepted=false,freeze=false,upsetWatch=false;
    {std::lock_guard<std::mutex> guard(historyMutex);accepted=pregame||gameHistory.observe(*liveSettings,s);freeze=gameHistory.comeback&&attendance::inferiorOpponent(s)<1;upsetWatch=gameHistory.upsetWatch;}
    if(!accepted)return;
    shownSuperSim=sim;
    if(sim&&replay.valid)simulationRecording=replay.recording;
    if(pregame||nativeUnpaused()){
        std::lock_guard<std::mutex> guard(historyMutex);
        departureModel.observe(*liveSettings,s,gameHistory,mode==0);
    }
    bool changeWindow=(pregame||nativeUnpaused())&&crowdChangeWindow.take(match,visualState,sim);
    if(changeWindow&&!pregame){std::lock_guard<std::mutex> guard(historyMutex);warning=gameHistory.takeWarning();}
    if(freeze!=shownComeback.load())log("protection",freeze?"Comeback protection active; crowd departures are reduced or paused.":"Comeback protection ended; future checkpoints may remove more fans.");
    shownComeback=freeze;shownUpset=upsetWatch;
    audioEmphasis=1;
    lastQuarter=s.quarter;lastClock=s.seconds;lastScore0=s.homeScore;lastScore1=s.awayScore;
    int command=pregame?0:request.exchange(0);const auto c=std::atomic_load(&settings);
    if(command==5){mode=0;restoreAt=0;} // Automatic changes settle at the next safe selection/break.
    if(command==2||command==4){mode=1;currentLoss=command==4?100.0f:50.0f;highestTarget=currentLoss;lastApplied=-1;restoreAt=command==2?now+12000:0;}
    if(command==1||command==3){mode=2;currentLoss=highestTarget=0;lastApplied=-1;restoreAt=0;}
    if(restoreAt&&now>=restoreAt){mode=2;currentLoss=highestTarget=0;lastApplied=-1;restoreAt=0;}
    if(!c->enabled){currentLoss=highestTarget=0;}
    if(!changeWindow&&(mode!=0||!c->enabled)){
        std::lock_guard<std::mutex> guard(historyMutex);
        auto finished=arrivalModel.finish();if(finished.changed||arrivalMissing>0)arrivalReconcilePending=true;
        arrivalMissing=0;shownHalftime=shownArriving=0;
    }
    if(changeWindow){
    {std::lock_guard<std::mutex> guard(historyMutex);departureModel.beginWindow(s,sim?100:c->transitionCap);departureModel.anchorVisible(static_cast<int>(currentLoss),arrivalMissing.load());}
    attendance::arrivals::Change arriving;
    {std::lock_guard<std::mutex> guard(historyMutex);
        auto arrivals=arrivalSettings();
        arriving=mode==0&&c->enabled?arrivalModel.observe(arrivals,s):arrivalModel.finish();
    }
    if(arriving.starting){
        {std::lock_guard<std::mutex> guard(historyMutex);departureModel.openingVisible();}
        captureArrivalBaseline=true;
        if(!guardedApply(false,false,true)){enabled=false;log("fault","Starting crowd could not be captured; arrivals stopped.");return;}
        log("arrivals",std::string(pregame?"Pregame":"First-quarter")+" arrival target: starting at "+std::to_string(100-arriving.missing)+"% of the game's original crowd; later arrivals settle at play selection.");
    }
    if(arriving.changed){
        arrivalReconcilePending=true;
        if(!arriving.starting)log("arrivals",arriving.finished?"First-quarter arrivals complete; target is the game's original crowd.":"First-quarter arrival target: "+std::to_string(100-arriving.missing)+"% of the original crowd.");
    }
    if(mode==0&&c->enabled)temporarySeats(s,static_cast<int>(currentLoss));
    else {arrivalMissing=0;shownHalftime=shownArriving=0;}
    if(warning&&s.quarter==4&&contextReady&&c->warningEnabled&&c->enabled&&mode==0)checkpoint(*c,s,true);
    else if(warning&&s.quarter==4&&!contextReady&&c->warningEnabled&&mode==0)log("checkpoint-skipped","Two-minute departure skipped because matchup information was unavailable.");
    int extra=0;{std::lock_guard<std::mutex> guard(atmosphereMutex);extra=pendingLateExits;pendingLateExits=0;}
    if(extra&&c->enabled&&mode==0&&!s.playoff&&s.quarter==4){
        int before=static_cast<int>(currentLoss);attendance::departures::Result late;
        {std::lock_guard<std::mutex> guard(historyMutex);late=departureModel.settle(s,before,extra,c->quarterCap);}
        currentLoss=highestTarget=static_cast<float>(late.loss);
        log("late-score","Late-score departure: crowd reduced from "+std::to_string(100-before)+"% to "+std::to_string(100-static_cast<int>(currentLoss))+"% of the original crowd.");
    }
    if(!pregame&&mode==0&&c->enabled){
        attendance::departures::Result gradual;std::string why;
        {std::lock_guard<std::mutex> guard(historyMutex);
            gradual=departureModel.settle(s,static_cast<int>(currentLoss),0,c->quarterCap);
            why=attendance::departures::reason(*c,s,gameHistory,gradual.catchup);
        }
        if(gradual.changed){
            int before=static_cast<int>(currentLoss);currentLoss=highestTarget=static_cast<float>(gradual.loss);
            log("departures",std::string(sim?"Super Sim departures: ":"Between-play departures: ")+"crowd reduced from "+std::to_string(100-before)+"% to "+std::to_string(100-gradual.loss)+"% ("+why+").");
            // Recompute temporary absences against the new permanent target.
            temporarySeats(s,gradual.loss);
        }
    }
    } // Settle at selection, pregame or verified offscreen Sim, never while lined up.
    lastFrame=now;shownCurrent=currentLoss;shownTarget=highestTarget;
    int percent=static_cast<int>(currentLoss);
    bool reconcile=arrivalReconcilePending.load();
    if((percent!=lastApplied||reconcile||visibilityGeneration!=generation||visibilityRetry)&&(!seatPatternPending||pregame||changeWindow||command||exactVisibility||mode!=0||!c->enabled)){
        reconcile=arrivalReconcilePending.exchange(false);
        removalLimit=static_cast<unsigned int>(percent*65536u/100u);activeLoss=percent;
        if(!guardedApply(percent>0||reconcile||arrivalMissing>0,reconcile)){enabled=false;log("fault","Native crowd update failed; automatic updates stopped");return;}
        lastApplied=percent;
        visibilityGeneration=generation;
        if(!visibilityRetry)exactVisibility=false;
    }
}
bool guardedDecision(){__try {decision();return true;}__except(EXCEPTION_EXECUTE_HANDLER){applying=false;thinning=false;return false;}}
void quarterBreak(int previous,int next){
    if(!enabled||mode!=0||replaySnapshot().active||next<=previous||previous<1||next>4||(pauseBinding&&!nativeUnpaused()))return;
    attendance::State s;A match=0;if(!gameState(s,match)||s.quarter!=next)return;
    auto c=std::atomic_load(&settings);if(!c->enabled)return;
    if(!guardedContext(s)){log("checkpoint-skipped","Quarter departure skipped because matchup information was unavailable.");return;}
    s.variationSeed=attendanceSeed;
    showContext(*c,s);
    attendance::arrivals::Change arrivals;
    {std::lock_guard<std::mutex> guard(historyMutex);if(!gameHistory.observe(*c,s,true)||!gameHistory.takeQuarter(previous,next))return;arrivals=arrivalModel.finish();}
    if(arrivals.changed){arrivalReconcilePending=true;log("arrivals","First-quarter arrivals complete at the quarter break; normal departures now apply.");}
    audioEmphasis=1;
    lastMatch=match;lastGeneration=entityGeneration.load();lastQuarter=next;lastClock=s.seconds;lastScore0=s.homeScore;lastScore1=s.awayScore;
    audioMatch=match;
    {std::lock_guard<std::mutex> guard(historyMutex);departureModel.beginWindow(s,c->transitionCap);departureModel.anchorVisible(static_cast<int>(currentLoss),arrivalMissing.load());}
    checkpoint(*c,s,false);
    shownQuarter=s.quarter;shownSeconds=s.seconds;shownScore0=s.homeScore;shownScore1=s.awayScore;shownPhase=s.phase;
}
void guardedQuarterBreak(int previous,int next){__try {quarterBreak(previous,next);}__except(EXCEPTION_EXECUTE_HANDLER){applying=false;thinning=false;enabled=false;}}
void __fastcall periodChanged(A manager,int next){
    int previous=at<int>(manager+8);
    if(enabled&&!replaySnapshot().active&&next==1&&manager==at<A>(image+attendance::bindings::ClockManager)){
        A wrapper=at<A>(image+attendance::bindings::MatchWrapper),match=at<A>(wrapper+16);bool keep=false;
        {std::lock_guard<std::mutex> guard(historyMutex);keep=matchLifecycle.keepAtKickoff(match);}
        if(!keep){attendance::State opening;A actual=0;if(gameState(opening,actual)&&actual==match)keep=carryPreparedOpening(match,opening);}
        if(!keep){
            if(!guardedApply(false,true)){enabled=false;log("fault","Previous crowd could not be released at kickoff.");}
            resetMatch();log("new-game","Starting crowd and departure history reset");
        }
        else log("new-game","Kickoff: preserving the crowd already prepared during pregame.");
    }
    if(enabled&&next>=2&&manager==at<A>(image+attendance::bindings::ClockManager))collectingStartingCrowd=false;
    originalPeriod(manager,next);
    if(manager==at<A>(image+attendance::bindings::ClockManager))guardedQuarterBreak(previous,next);
}
void readResumeStatus(uint64_t now){
    attendance::resume::Signal next;
    try{
        auto path=pluginsFolder/L"MidgameSave"/L"midgamesave-status.json";
        std::error_code error;auto size=std::filesystem::file_size(path,error);
        if(!error&&size>0&&size<=65536){
            auto modified=std::filesystem::last_write_time(path,error);
            auto age=std::filesystem::file_time_type::clock::now()-modified;
            if(!error&&age>=-std::chrono::seconds(2)&&age<=std::chrono::seconds(10)){
                std::ifstream f(path,std::ios::binary);std::string text((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
                if(text.size()<=65536)next=attendance::resume::parse(text,static_cast<int>(GetCurrentProcessId()),now);
            }
        }
    }catch(...){/* Missing/partial foreign status never blocks ordinary games. */}
    std::shared_ptr<const attendance::resume::Signal> signal=std::make_shared<attendance::resume::Signal>(next);std::atomic_store(&resumeSignal,signal);
}
void loadAttendanceHistory(){
    auto path=pluginsFolder/L"DynamicGamedayAtmosphere"/L"History"/L"attendance-history.txt";
    std::error_code error;auto size=std::filesystem::file_size(path,error);if(error||size>8*1024*1024)return;
    std::ifstream f(path);attendance::timeline::Archive archive;
    if(attendance::timeline::read(f,archive))attendanceArchive=std::move(archive);
    else log("history","Unreadable crowd history ignored; resumed games will use an estimate.");
}
void saveAttendanceHistory(){
    static uint64_t written=0;attendance::timeline::Archive snapshot;
    {std::lock_guard<std::mutex> guard(timelineMutex);if(attendanceArchive.revision==written)return;snapshot=attendanceArchive;}
    auto dir=pluginsFolder/L"DynamicGamedayAtmosphere"/L"History";std::error_code error;std::filesystem::create_directories(dir,error);if(error)return;
    auto file=dir/L"attendance-history.txt",pending=dir/L"attendance-history.pending";
    {std::ofstream f(pending,std::ios::trunc);attendance::timeline::write(f,snapshot);f.flush();if(!f)return;}
    if(MoveFileExW(pending.c_str(),file.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))written=snapshot.revision;
}
DWORD WINAPI worker(void*) {
    wchar_t path[32768]{};GetModuleFileNameW(self,path,32768);folder=std::filesystem::path(path).parent_path();pluginsFolder=folder.parent_path().parent_path();logsFolder=pluginsFolder/L"DynamicGamedayAtmosphere"/L"Logs";image=reinterpret_cast<A>(GetModuleHandleW(nullptr));
    lastApplied=0;shownCurrent=shownTarget=0;
    readSettings();log("starting","Dynamic Gameday Atmosphere " GAMEDAY_VERSION "; attendance, retention and crowd audio.");
    if(GetPrivateProfileIntW(L"General",L"ConfigVersion",GAMEDAY_SCHEMA,(pluginsFolder/L"DynamicGamedayAtmosphere.ini").c_str())>GAMEDAY_SCHEMA){
        log("inactive","Settings belong to a newer version. Update the plugin before using this configuration.");return 1;
    }
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,GetCurrentProcessId());
    if(modules==INVALID_HANDLE_VALUE){log("inactive","Could not check for another atmosphere runtime; no hooks installed.");return 1;}
    MODULEENTRY32W module{};module.dwSize=sizeof(module);bool conflict=false;
    if(Module32FirstW(modules,&module))do{
        if(module.hModule==self)continue;
        const std::wstring name=module.szModule;
        if(name.find(L"DynamicAttendanceNative")==0||name.find(L"DynamicAttendanceProbe")==0||name.find(L"DynamicGamedayAtmosphereNative")==0)conflict=true;
    }while(Module32NextW(modules,&module));
    CloseHandle(modules);
    if(conflict){log("inactive","An older attendance/atmosphere runtime is already loaded. Restart both MMC and the game; no new hooks installed.");return 1;}
    const std::wstring mutexName=L"Local\\Ace.DynamicGamedayAtmosphere.Game."+std::to_wstring(GetCurrentProcessId());
    HANDLE lifetime=CreateMutexW(nullptr,FALSE,mutexName.c_str());
    if(!lifetime||GetLastError()==ERROR_ALREADY_EXISTS){if(lifetime)CloseHandle(lifetime);log("inactive","Another atmosphere runtime already owns this game.");return 1;}
    loadAttendanceHistory();readResumeStatus(GetTickCount64());
    if(!install()){log("inactive",signatures?"No compatible crowd entities or hook installation failed":"Required native bytes differ; no hooks installed");return 1;}
    log("ready","Ready. First-quarter arrivals fill toward the game's original attendance. Formula-based departures settle between plays and at breaks, with the largest exit window at halftime.");
    log(playBindingsAvailable?"ready":"audio-inactive",playBindingsAvailable?"Opening roar and winning eruptions ready. Normal volume boost limit about +58%; winning eruptions about +100%.":"Play data unavailable; winning eruptions and late-score departures are inactive.");
    if(replayPresentationInstalled&&extraReplays)log("ready","Extra post-play replays ready: routine run/pass insets and full-screen big-play sequences at "+std::to_string(replayMinimumYards.load())+" yards; defensive highlights "+(defensiveReplays?"enabled":"disabled")+". Normal replay playback and skipping remain active.");
    unsigned long long last=0,lastScan=0;bool replayFaultNoted=false;
    while(enabled){
        // UI/loading may suspend presentation callbacks while quitting a game.
        // Remember that exit even if the next game reuses the same match object.
        observeWorldExit();
        std::string command;{std::ifstream f(pluginsFolder/L"DynamicGamedayAtmosphere-control.txt");std::getline(f,command);}
        if(!command.empty()){
            {std::ofstream clear(pluginsFolder/L"DynamicGamedayAtmosphere-control.txt",std::ios::trunc);}
            if(command=="observe")request=1;
            else if(command=="probe")request=2;
            else if(command=="empty")request=4;
            else if(command=="auto")request=5;
            else if(command=="restore"||command=="stop")request=3;
            log("requested",command);
        }
        auto now=GetTickCount64();if(now-last>1000){
            if(replayPresentationFailed&&!replayFaultNoted){log("fault","Extra post-play replays stopped after a write fault; normal game presentation remains active.");replayFaultNoted=true;}
            readResumeStatus(now);saveAttendanceHistory();
            readSettings();std::error_code logError;std::filesystem::create_directories(logsFolder,logError);std::ofstream state(logsFolder/L"status.txt",std::ios::trunc);
            auto audio=std::atomic_load(&coreSettings);
            state<<std::fixed<<std::setprecision(1)<<"Dynamic Gameday Atmosphere " GAMEDAY_VERSION
              <<"\nGame: Q"<<shownQuarter<<" | "<<shownSeconds<<" seconds remaining | Home "<<shownScore0<<" - Away "<<shownScore1
              <<"\nMode: "<<(mode==0?"automatic":mode==1?"manual test":"restored")
              <<"\nCrowd target: "<<(100-(std::max)(activeLoss.load(),arrivalMissing.load()))<<"% of original attendance"
              <<"\nSeating pattern: "<<shownSeatPatterns<<" area-based crowd(s), "<<shownSeatFallbacks<<" scattered fallback(s)"<<(seatPatternPending?"; waiting for native seats":"")
              <<"\nReplay crowd: "<<(replayLoss>=0?std::to_string(100-replayLoss.load())+(replaySuspendsAtmosphere?"% (interactive; live target preserved)":"% (automatic; live target preserved)"):"inactive")
              <<"\nExtra post-play replays: "<<(!replayPresentationInstalled?"unavailable":replayPresentationFailed?"stopped after a fault":!extraReplays?"disabled":"enabled")
              <<" | Routine insets: "<<(routineInsets?"on":"off")<<" | Full-screen gains: "<<(bigPlayReplays?std::to_string(replayMinimumYards.load())+"+ yards":"off")
              <<" | Defensive highlights: "<<(defensiveReplays?"on":"off")
              <<"\nScenario attendance: "<<(shownResumeSource==2?"restored from crowd history":shownResumeSource==1?"estimated at resumed start":"ordinary game")
              <<"\nCrowd measured: "<<(visibilityMeasured?std::to_string(100.0*static_cast<double>(shownVisibleFans.load())/static_cast<double>((std::max)(1ull,shownOriginalFans.load())))+"%":"waiting for renderer")
              <<"\nCrowd renderer: "<<(visibilityRetry?"waiting for crowd update":visibilityMeasured?"verified":"waiting for first measurement")
              <<"\nPermanent departures: "<<activeLoss<<"% | Still arriving: "<<shownArriving<<"% | Halftime seat break: "<<shownHalftime<<"%"
              <<"\nSuper Sim crowd updates: "<<(!superSimBinding?"unavailable":shownSuperSim?"offscreen simulation":"waiting for selection, break or simulation")
              <<"\nTeam IDs (home/away): "<<shownTeam0<<" / "<<shownTeam1<<" | Ranks: "<<shownRank0<<" / "<<shownRank1<<" | Rivalry: "<<(shownRivalry?"yes":"no")
              <<"\nDeparture formula: "<<(shownPlayoff?"postseason (blowouts only)":attendance::profileName(static_cast<attendance::Profile>(shownProfile.load())))
              <<"\nComeback protection: "<<(shownComeback?"active":"inactive")<<" | Upset retention: "<<(shownUpset?"active":"inactive")
              <<"\nCrowd audio: "<<(!audioHookInstalled?"unavailable":!audioSetting||!audio->enabled?"disabled":!crowdAudio.healthy()?"stopped after a fault":"enabled")
              <<"\nAttendance volume cut: "<<100*(1-crowdAudio.gain())<<"% | Begins below "<<audioReductionStart<<"% attendance | Six-second fade"
              <<"\nGeneral crowd volume boost: +"<<attendance::volume::boostPercent(crowdBoostAudio.db())<<"%"
              <<"\nSteady ambience volume boost: +"<<attendance::volume::boostPercent(crowdBoostAudio.db()+ambienceAudio.db())<<"% total"
              <<"\nBoost limits: normal +"<<attendance::volume::boostPercent(attendance::core::normalLimit(*audio))<<"% | eruptions +"<<attendance::volume::boostPercent(attendance::core::eruptionLimit(*audio))<<"%"
              <<"\nAdded crowd roar: "<<(!audio->enabled||(!audio->opening&&!audio->overtime&&!audio->eruptions)?"disabled":!swellAvailable?"unavailable":!sustainedAudio.healthy()?"stopped after a fault":sustainedAudio.playing()?"playing":"inactive")
              <<"\nNative celebration cheers requested: "<<positiveCheer.sent()<<" | Cheer binding: "<<(cheerAvailable&&positiveCheer.healthy()?"ready":"unavailable")
              <<"\nWinning eruption: "<<((shownEruption&&now<shownDecisiveUntil.load())?"active":"inactive")
              <<"\nPostgame celebration carry: "<<(shownPostgameCarry?"active":"inactive")
              <<"\nExtra chant volume cut: "<<attendance::volume::cutPercent(chantAudio.db())<<"% (no extra cut at 70% attendance or above)"
              <<"\n";
            if(debugLog)state<<"Debug: callbacks="<<ticks<<", replay callbacks="<<replayCallbacks<<", renderer calls="<<shownVisibilityCalls<<", roar starts/stops="<<sustainedAudio.starts()<<"/"<<sustainedAudio.stops()<<", inset/full/defense eligibility changes="<<insetEligibilityChanges<<"/"<<fullEligibilityChanges<<"/"<<defensiveRouteChanges<<" (repeated evaluations, not replay totals)\n";
            last=now;
        }
        if(now-lastScan>5000){
            bool needs=refreshEntities.exchange(false);{std::lock_guard<std::mutex> guard(entityMutex);if(entities.empty())needs=true;for(A e:entities)if(!entityValid(e))needs=true;}
            attendance::State s;A match=0;if(needs&&gameState(s,match))findEntities();lastScan=now;
        }
        Sleep(100);
    }
    return 0;
}
}
extern "C" void CrowdTick() {
    ++ticks;
    if(!enabled)return;
    std::unique_lock<std::mutex> frame(presentationMutex,std::try_to_lock);if(!frame.owns_lock())return;
    bool replay=guardedReplayPresentation();
    if(!replay&&!guardedDecision()){enabled=false;log("fault","Attendance callback failed; automatic updates stopped");}
    if(enabled&&!replay&&!guardedRecordAttendance())log("history","Crowd snapshot unavailable; existing history preserved.");
    if(enabled&&!guardedAtmosphereTick()){playBindingsAvailable=false;crowdBoost=0;log("audio-inactive","Play data became unavailable; winning-event updates stopped. Quarter departures remain active.");}
}
extern "C" __declspec(dllexport) DWORD WINAPI DynamicGamedayAtmosphereStop(void*){request=3;return 0;}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID) {
    if(reason==DLL_PROCESS_ATTACH){self=instance;DisableThreadLibraryCalls(instance);HANDLE t=CreateThread(nullptr,0,worker,nullptr,0,nullptr);if(t)CloseHandle(t);}
    return TRUE;
}
