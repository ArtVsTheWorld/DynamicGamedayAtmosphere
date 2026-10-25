// Run the actual production visibility/apply/cache functions in this test
// executable, with native scene callbacks replaced by private test objects.
// No running game, injection or external process handle is involved.
#include "../Source/NativeRuntime.cpp"
#include <cassert>
#include <iostream>
namespace {
template<class T>void testPut(A address,T value){std::memcpy(reinterpret_cast<void*>(address),&value,sizeof(value));}
struct TestScene {
    std::vector<A> object,renderer,properties,entry,value,holder;
    std::vector<unsigned short> masks,native,original;
    std::vector<attendance::seats::Position> xyz;
    TestScene(unsigned n,bool mapped=true):object(0xa00/8),renderer(0x200/8),properties(2),entry(2),value(2),holder(2),masks(n),native(n),original(n),xyz(n){
        for(unsigned i=0;i<n;++i){float row=static_cast<float>((i/24)%32),seat=static_cast<float>(i%24);xyz[i]={28+row*.9f,1+row*.65f,-48+seat*4,0};
            masks[i]=native[i]=original[i]=static_cast<unsigned short>(i%7==0?7:(i&1)?4:0);
        }
        testPut(address(),image+attendance::bindings::SceneCrowdVtable);testPut(address()+0x9f0,render());testPut(address()+0x908,A(0x10000));
        testPut(render(),image+attendance::bindings::CrowdRendererVtable);buffers();
        if(mapped){testPut(address()+0x9f8,reinterpret_cast<A>(properties.data()));properties[0]=reinterpret_cast<A>(entry.data());properties[1]=properties[0]+16;
            entry[0]=image+attendance::bindings::CrowdPositionKey;entry[1]=reinterpret_cast<A>(value.data());value[1]=reinterpret_cast<A>(holder.data());holder[0]=n;holder[1]=reinterpret_cast<A>(xyz.data());}
    }
    A address()const{return reinterpret_cast<A>(object.data());}A render()const{return reinterpret_cast<A>(renderer.data());}
    void buffers(){testPut(render()+0x188,reinterpret_cast<A>(masks.data()));testPut(render()+0x190,reinterpret_cast<A>(masks.data()+masks.size()));}
};
std::vector<TestScene*> testScenes;
bool nativePresentationActive=false;
unsigned __fastcall testPart(A,unsigned part){return part;}
void __fastcall testSet(A scene,unsigned part,unsigned agent,bool hidden){
    for(auto* s:testScenes)if(s->address()==scene){auto flag=static_cast<unsigned short>(1u<<part);s->masks[agent]=static_cast<unsigned short>((s->masks[agent]&~flag)|(hidden?flag:0));return;}
    assert(false);
}
void __fastcall testUpdate(A){
    if(applying)assert(!nativePresentationActive);
    for(auto* s:testScenes)for(unsigned i=0;i<s->masks.size();++i)for(unsigned part=0;part<3;++part){
        // One model part only exists on even-numbered agents. Baseline readiness
        // must not require that part's callback for every agent.
        if(part==2&&(i&1))continue;
        visibility(s->address(),part,i,(s->native[i]&(1u<<part))!=0);
    }
}
void __fastcall testPresentation(A,float){
    nativePresentationActive=true;
    for(A controller:entities)attendanceUpdate(controller);
    nativePresentationActive=false;
}
void checkScene(const TestScene& s,int loss){
    const auto& base=startingCrowds.at(s.render());assert(base.selection.policy!=attendance::seats::Policy::Pending);
    for(unsigned i=0;i<s.masks.size();++i){bool selected=base.selection.departed(i,loss);
        for(unsigned part=0;part<3;++part){if(part==2&&(i&1))continue;auto flag=static_cast<unsigned short>(1u<<part);
            bool expected=(s.native[i]&flag)||(s.original[i]&flag)||selected;assert(((s.masks[i]&flag)!=0)==expected);
        }
    }
}
std::vector<void*> worldPages;
void testPage(A address){
    A page=address&~A(0xffff);for(auto p:worldPages)if(reinterpret_cast<A>(p)==page)return;
    auto p=VirtualAlloc(reinterpret_cast<void*>(page),65536,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);assert(p==reinterpret_cast<void*>(page));worldPages.push_back(p);
}
template<class T>void testGlobal(A rva,T value){testPage(image+rva);testPut(image+rva,value);}
template<class F>void testFunction(A rva,F function){
    testPage(image+rva);unsigned char thunk[]={0x48,0xb8,0,0,0,0,0,0,0,0,0xff,0xe0};A target=reinterpret_cast<A>(function);
    std::memcpy(thunk+2,&target,8);std::memcpy(reinterpret_cast<void*>(image+rva),thunk,sizeof(thunk));FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(image+rva),sizeof(thunk));
}
struct TestVariant {A vt,type,data;};
bool __fastcall testOption(A,unsigned key,TestVariant* value,A,A){
    static unsigned short bowl=127,rivalry=1023;
    value->data=reinterpret_cast<A>(key==0x9e2b2900?&bowl:&rivalry);return true;
}
bool __fastcall testNeutral(){return false;}
int __fastcall testRank(A,int team){return team>=1500?255:26;}
void __fastcall testPeriod(A manager,int next){testPut(manager+8,next);}
// Build the same verified read-only world layout in THIS executable, allowing
// decision() and the native attendance wrapper to be exercised before a snap.
struct TestWorld {
    std::vector<A> config,states,wrapper,match,manager,clock,weather,service;
    unsigned char paused=0;
    TestWorld():config(0x100/8),states(0xb0/8),wrapper(4),match(0x200/8),manager(0x40/8),clock(0x40/8),weather(0x400/8),service(2){
        testGlobal(0xd9e3818,14);testGlobal(0xf3f5d48,3);testGlobal(0xf6379e0,reinterpret_cast<A>(config.data()));
        testGlobal(attendance::bindings::GameStates,reinterpret_cast<A>(states.data()));states[0]=image+attendance::bindings::GameStatesVtable;phase(6);
        testGlobal(attendance::bindings::MatchWrapper,reinterpret_cast<A>(wrapper.data()));wrapper[2]=reinterpret_cast<A>(match.data());match[0]=image+attendance::bindings::MatchVtable;
        testGlobal(attendance::bindings::ClockManager,reinterpret_cast<A>(manager.data()));manager[0]=image+0xb4787a8;testPut(reinterpret_cast<A>(manager.data())+8,0);manager[5]=reinterpret_cast<A>(clock.data());
        testPut(reinterpret_cast<A>(clock.data())+0x28,900.0f);testPut(reinterpret_cast<A>(clock.data())+0x2c,900.0f);
        testGlobal(0xd9f3970,1119);testGlobal(0xd9f3974,1503);testGlobal(attendance::bindings::PauseFlag,reinterpret_cast<A>(&paused));pauseBinding=true;
        testGlobal(0xf4fe000,reinterpret_cast<A>(weather.data()));weather[0]=image+0xb323ed0;testPut(reinterpret_cast<A>(weather.data())+0xe8,70);
        testGlobal(0xe2dd3e8,reinterpret_cast<A>(service.data()));service[0]=image+0xb298a70;testPage(service[0]);testPut(service[0]+0x68,image+0x90934d0);
        testGlobal(0xfc88ab8,A(0));testFunction(0x90934d0,testOption);testFunction(attendance::bindings::NeutralGetter,testNeutral);testFunction(0xa0ad090,testRank);
    }
    void phase(int value){testPut(reinterpret_cast<A>(states.data())+0xa0,value);}
};
}
int main(){
    image=0x10000000;enabled=true;originalSet=testSet;originalAttendanceUpdate=testUpdate;
    logsFolder=std::filesystem::temp_directory_path()/L"Ace-SeatRuntimeTests";std::filesystem::create_directories(logsFolder);
    // A resolver shim in this executable's own memory supplies the verified
    // native ABI expected by the production part resolver.
    A resolver=image+0x65047b0,page=resolver&~A(0xffff);void* allocation=VirtualAlloc(reinterpret_cast<void*>(page),65536,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);assert(allocation==reinterpret_cast<void*>(page));
    unsigned char thunk[]={0x48,0xb8,0,0,0,0,0,0,0,0,0xff,0xe0};A target=reinterpret_cast<A>(&testPart);std::memcpy(thunk+2,&target,8);std::memcpy(reinterpret_cast<void*>(resolver),thunk,sizeof(thunk));FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(resolver),sizeof(thunk));
    std::vector<A> controller(8),link(2);controller[0]=image+0xb47ec48;controller[7]=reinterpret_cast<A>(link.data());link[1]=image+0xf502b30;entities={reinterpret_cast<A>(controller.data())};
    TestScene bowl(1536),deck(1536),sideline(60);for(auto& p:deck.xyz)p.y+=15;testScenes={&bowl,&deck,&sideline};
    assert(guardedApply(false,false,true));assert(shownSeatPatterns==2&&shownSeatFallbacks==1&&!seatPatternPending);
    assert(startingCrowds.at(bowl.render()).selection.policy==attendance::seats::Policy::Position);
    // Natural sideline visibility may be observed once even though this
    // attendance controller never visits that small draw in a baseline pass.
    auto& idleSmall=startingCrowds.at(sideline.render());idleSmall.baselineComplete=false;idleSmall.selection={};
    prepareSeatPatterns();assert(!seatPatternPending&&idleSmall.selection.policy==attendance::seats::Policy::Pending);
    idleSmall.baselineComplete=true;prepareSeatPatterns();assert(idleSmall.selection.policy==attendance::seats::Policy::Scattered);
    auto order=startingCrowds.at(bowl.render()).selection.rank;
    auto settle=[&](int permanent,int temporary){activeLoss=permanent;arrivalMissing=temporary;assert(guardedApply(true,true));for(auto* s:testScenes)checkScene(*s,(std::max)(permanent,temporary));};
    settle(0,55);settle(0,25);settle(0,0);settle(35,45);settle(35,35);settle(35,0);
    assert(startingCrowds.at(bowl.render()).selection.rank==order);
    for(int earlier:{5,70,20,0,55,10}){replayLoss=earlier;assert(guardedApply(true,true));for(auto* s:testScenes)checkScene(*s,earlier);}
    replayLoss=-1;exactVisibility=true;assert(guardedApply(true,true));for(auto* s:testScenes)checkScene(*s,35);exactVisibility=false;
    assert(shownExpectedFans==shownVisibleFans&&shownOtherExtra==0&&shownExtraVisible==0);
    TestScene late(1536);testScenes.push_back(&late);assert(guardedApply(true,true));assert(seatPatternPending);assert(startingCrowds.at(late.render()).selection.policy==attendance::seats::Policy::Pending);
    assert(guardedApply(true,true));assert(!seatPatternPending&&shownSeatPatterns==3);for(auto* s:testScenes)checkScene(*s,35);
    // New visibility buffer at the same renderer must recapture native seats,
    // never reinterpret old plugin holes as permanent original empties.
    auto old=std::move(bowl.masks);bowl.masks=bowl.native;bowl.buffers();assert(guardedApply(true,true));assert(seatPatternPending);assert(guardedApply(true,true));assert(!seatPatternPending);
    assert(startingCrowds.at(bowl.render()).selection.rank==order);checkScene(bowl,35);
    // Retired crowd cannot keep other renderers perpetually pending.
    testScenes.pop_back();testPut(late.address(),A(0));assert(guardedApply(true,true));assert(startingCrowds.find(late.render())==startingCrowds.end());
    // Scenario cleanup must win over a stale exact/historical replay request.
    mode=2;replayLoss=70;exactVisibility=true;assert(guardedApply(false,true));for(auto* s:testScenes)checkScene(*s,0);
    resetMatch();assert(startingCrowds.empty()&&!seatPatternPending&&shownSeatPatterns==0);
    mode=0;assert(guardedApply(false,false,true));assert(startingCrowds.at(bowl.render()).selection.rank==order);settle(60,0);
    mode=2;assert(guardedApply(false,true));
    // Late renderer: the intro's single arrival window was consumed while no
    // native crowd existed. Repeated PREgame decisions must finish setup now,
    // without a quarter transition, play selection or a first snap.
    resetMatch();mode=0;entities.clear();testScenes.clear();TestWorld world;originalPeriod=testPeriod;lastDecision=0;
    assert(guardedDecision());int opening=arrivalMissing.load();assert(opening>0&&visibilityRetry&&arrivalModel.active);
    assert(!crowdChangeWindow.pending); // No new ordinary crowd-change window.
    TestScene intro(1536);testScenes={&intro};testUpdate(0);assert(seatPatternPending);originalPresentationUpdate=testPresentation;
    entities={reinterpret_cast<A>(controller.data())};lastDecision=0;assert(guardedDecision());
    assert(!seatPatternPending&&shownSeatPatterns==1);checkScene(intro,opening);assert(arrivalMissing==opening);
    // A native attendance callback may replace masks later in the same intro.
    auto oldIntro=std::move(intro.masks);intro.masks=intro.native;intro.buffers();lastDecision=GetTickCount64();
    presentationUpdate(reinterpret_cast<A>(world.states.data()),.016f);
    assert(!seatPatternPending);checkScene(intro,opening);
    assert(arrivalMissing==opening); // Same intro cannot advance arrivals.
    auto openingOrder=startingCrowds.at(intro.render()).selection.rank;
    // The engine can replace the match object when handing pregame to kickoff.
    // Same teams + no snap + full Q1 clock must preserve the prepared picture.
    auto kickoffMatch=world.match;world.wrapper[2]=reinterpret_cast<A>(kickoffMatch.data());
    periodChanged(reinterpret_cast<A>(world.manager.data()),1);
    assert(lastMatch==world.wrapper[2]&&arrivalMissing==opening&&arrivalModel.active);
    assert(startingCrowds.at(intro.render()).selection.rank==openingOrder);checkScene(intro,opening);
    world.phase(1);lastDecision=0;assert(guardedDecision());assert(arrivalMissing==opening);
    auto activeMatch=kickoffMatch;world.wrapper[2]=reinterpret_cast<A>(activeMatch.data());lastDecision=0;
    assert(guardedDecision());assert(lastMatch==world.wrapper[2]&&arrivalMissing==opening);checkScene(intro,opening);
    // Midgame Save publishes its ordinary new-game number only at preplay,
    // after this plugin has prepared the intro. That unarmed acknowledgement
    // must not reset arrivals or refill the renderer before the opening snap.
    resumeTracker={};
    auto publishOpening=[&](int number){
        auto next=std::make_shared<attendance::resume::Signal>();next->valid=true;next->game=number;next->received=GetTickCount64();
        next->live={1,0,0,1,900,900};std::shared_ptr<const attendance::resume::Signal> value=next;std::atomic_store(&resumeSignal,value);
    };
    publishOpening(10);lastDecision=0;assert(guardedDecision());auto preparedCaptures=captureSequence;
    publishOpening(11);assert(ordinaryOpeningAcknowledgement(lastMatch,{1,0,0,1,900,900}));
    lastDecision=0;assert(guardedDecision());
    assert(resumeTracker.game==11&&arrivalMissing==opening&&matchLifecycle.prepared&&captureSequence==preparedCaptures);
    assert(startingCrowds.at(intro.render()).selection.rank==openingOrder);checkScene(intro,opening);
    std::atomic_store(&resumeSignal,std::make_shared<const attendance::resume::Signal>());
    // Quit with presentation suspended: the stage may remain 14 while the
    // secondary mode leaves 3. Then reuse the SAME match/render buffers for a
    // new intro with a different school and a completed old arrival curve.
    matchLifecycle.live();arrivalModel.finish();currentLoss=highestTarget=35;activeLoss=35;arrivalMissing=0;assert(guardedApply(true,true));
    testGlobal(0xf3f5d48,2);observeWorldExit();assert(matchLifecycle.leftWorld);testGlobal(0xf3f5d48,3);
    testGlobal(0xd9f3970,1212);testGlobal(0xd9f3974,1501);world.phase(6);testPut(reinterpret_cast<A>(world.manager.data())+8,0);lastDecision=0;
    assert(guardedDecision());int secondOpening=arrivalMissing.load();assert(secondOpening>opening&&arrivalModel.active&&!matchLifecycle.leftWorld);
    assert(activeLoss==0&&!seatPatternPending&&shownSeatPatterns==1);checkScene(intro,secondOpening);
    auto captures=captureSequence;for(int i=0;i<10;++i){lastDecision=0;assert(guardedDecision());assert(arrivalMissing==secondOpening);}
    assert(captureSequence==captures); // A ready intro does not rebuild or recapture per frame.
    // The immediate renderer retry is pregame-only. At the line, ordinary
    // changes still wait until the next selection/break.
    world.phase(1);testPut(reinterpret_cast<A>(world.manager.data())+8,1);
    auto secondBuffer=std::move(intro.masks);intro.masks=intro.native;intro.buffers();attendanceUpdate(reinterpret_cast<A>(controller.data()));
    assert(seatPatternPending);world.phase(5);lastDecision=0;assert(guardedDecision());assert(!seatPatternPending);checkScene(intro,secondOpening);
    // Exercise the actual decision/render path in offscreen Super Sim.
    std::vector<A> simController(4);simController[0]=image+attendance::bindings::SuperSimVtable;
    testGlobal(attendance::bindings::SuperSimController,reinterpret_cast<A>(simController.data()));superSimBinding=true;
    std::vector<A> replayManager(0x1040/8);replayManager[0]=image+attendance::bindings::ReplayVtable;
    testGlobal(attendance::bindings::ReplayManager,reinterpret_cast<A>(replayManager.data()));replayBinding=true;
    testPut(reinterpret_cast<A>(replayManager.data())+0x1030,100.0);
    std::vector<A> replayEntity(4);replayEntity[0]=image+attendance::bindings::ReplayEntityVtable;
    originalReplayUpdate=[](A){};
    world.phase(0);testPut(reinterpret_cast<A>(world.clock.data())+0x28,100.0f);lastDecision=0;
    replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(arrivalMissing==secondOpening); // None alone is rejected.
    testPut(reinterpret_cast<A>(simController.data())+8,1);testPut(reinterpret_cast<A>(simController.data())+12,1);lastDecision=0;
    replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(shownSuperSim&&arrivalMissing<secondOpening);checkScene(intro,arrivalMissing);
    auto observedTime=gameHistory.elapsed;
    world.paused=1;testPut(reinterpret_cast<A>(replayManager.data())+0x38,static_cast<unsigned char>(1));
    testPut(reinterpret_cast<A>(world.clock.data())+0x28,90.0f);lastDecision=0;
    replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(gameHistory.elapsed==observedTime);
    world.paused=0;testPut(reinterpret_cast<A>(replayManager.data())+0x38,static_cast<unsigned char>(0));
    replayUpdate(reinterpret_cast<A>(replayEntity.data()));
    // The live Normal/Fast trace held PlayCall for its ENTIRE play-by-play
    // sequence. Main presentation is suspended, so only replayUpdate runs.
    world.phase(5);testPut(reinterpret_cast<A>(world.manager.data())+8,2);
    testPut(world.wrapper[2]+0x1d8,35);testPut(world.wrapper[2]+0x1dc,0);
    auto simulateClock=[&](float clock){
        testPut(reinterpret_cast<A>(world.clock.data())+0x28,clock);lastDecision=0;
        replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(shownSuperSim);
        checkScene(intro,(std::max)(activeLoss.load(),arrivalMissing.load()));
    };
    simulateClock(900);assert(arrivalMissing==0&&arrivalModel.done);int beforeSim=activeLoss;
    for(float clock:{600.0f,300.0f,160.0f})simulateClock(clock);
    assert(shownHalftime==0&&activeLoss>beforeSim); // Permanent exits happen during the quarter.
    simulateClock(100);assert(shownQuarter==2&&halftimeModel.active&&shownHalftime>0);
    simulateClock(60);int earlyAbsent=shownHalftime;
    simulateClock(0);assert(shownHalftime>=earlyAbsent);int peakAbsent=shownHalftime;
    testPut(reinterpret_cast<A>(world.manager.data())+8,3);simulateClock(900);
    assert(shownHalftime>0&&shownHalftime<=peakAbsent);
    simulateClock(850);assert(shownHalftime>0&&shownHalftime<=peakAbsent);
    simulateClock(800);assert(shownHalftime>0&&shownHalftime<peakAbsent);
    simulateClock(650);assert(shownQuarter==3&&shownHalftime==0&&arrivalMissing==0&&halftimeModel.done);
    testPut(reinterpret_cast<A>(world.manager.data())+8,4);simulateClock(900);int q4SimStart=activeLoss;
    testPut(world.wrapper[2]+0x1d8,70);
    for(float clock:{800.0f,600.0f,300.0f,200.0f})simulateClock(clock);
    assert(shownTeam1>=1500&&activeLoss>q4SimStart&&activeLoss-q4SimStart<=45&&activeLoss<=75);
    auto lastSimTime=gameHistory.elapsed;int simLoss=activeLoss;
    world.paused=1;testPut(reinterpret_cast<A>(world.clock.data())+0x28,195.0f);lastDecision=0;
    replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(gameHistory.elapsed==lastSimTime&&activeLoss==simLoss);
    world.paused=0;
    auto historyBefore=attendanceArchive.revision;assert(simulationRecording==100);
    assert(guardedRecordAttendance());assert(attendanceArchive.revision==historyBefore);
    int afterSim=activeLoss;testPut(reinterpret_cast<A>(simController.data())+8,0);testPut(reinterpret_cast<A>(simController.data())+12,0);
    world.phase(1);testPut(reinterpret_cast<A>(world.clock.data())+0x28,190.0f);lastDecision=0;assert(guardedDecision());
    assert(!shownSuperSim&&activeLoss==afterSim); // No hidden pending window at the visible line.
    lastDecision=17;replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(lastDecision==17&&activeLoss==afterSim);
    assert(guardedRecordAttendance());assert(attendanceArchive.revision==historyBefore); // Frozen filming time preserves the earlier clip.
    world.phase(3);testPut(reinterpret_cast<A>(world.clock.data())+0x28,180.0f);lastDecision=0;assert(guardedDecision());assert(activeLoss==afterSim);
    world.phase(5);lastDecision=0;assert(guardedDecision());assert(activeLoss>=afterSim);checkScene(intro,(std::max)(activeLoss.load(),arrivalMissing.load()));
    lastDecision=23;replayUpdate(reinterpret_cast<A>(replayEntity.data()));assert(lastDecision==23); // Ordinary PlayCall is not offscreen Sim.
    testPut(reinterpret_cast<A>(replayManager.data())+0x1030,100.1);
    assert(guardedRecordAttendance());assert(attendanceArchive.revision>historyBefore&&simulationRecording<0);
    // Final presentation streams a new seated crowd after live play ends.
    // It must inherit the final picture without a new arrival/departure step.
    world.phase(7);testPut(reinterpret_cast<A>(world.manager.data())+8,6);testPut(reinterpret_cast<A>(world.clock.data())+0x28,0.0f);
    TestScene finalScene(1536);testScenes={&finalScene};int finalLoss=activeLoss,finalMissing=arrivalMissing;auto finalHazard=departureModel.hazard;
    attendanceUpdate(reinterpret_cast<A>(controller.data()));assert(seatPatternPending&&postgameRendererRetry);
    lastDecision=GetTickCount64();presentationUpdate(reinterpret_cast<A>(world.states.data()),.016f);
    assert(!seatPatternPending&&!visibilityRetry);checkScene(finalScene,(std::max)(finalLoss,finalMissing));
    assert(activeLoss==finalLoss&&arrivalMissing==finalMissing&&departureModel.hazard==finalHazard);
    auto finalCaptures=captureSequence;for(int i=0;i<5;++i)presentationUpdate(reinterpret_cast<A>(world.states.data()),.016f);
    assert(captureSequence==finalCaptures); // A ready postgame does not recapture every frame.
    world.phase(8);TestScene finalTransition(1536);testScenes={&finalTransition};
    attendanceUpdate(reinterpret_cast<A>(controller.data()));assert(postgameRendererRetry);
    lastDecision=GetTickCount64();presentationUpdate(reinterpret_cast<A>(world.states.data()),.016f);
    assert(!seatPatternPending&&!visibilityRetry);checkScene(finalTransition,(std::max)(finalLoss,finalMissing));
    assert(activeLoss==finalLoss&&departureModel.hazard==finalHazard); // EndOfGame + QuarterEnd uses the same final-only repair.
    mode=2;assert(guardedApply(false,true));
    // A late blowout queues the new catch-up in the actual production
    // decision path, but seats stay fixed until play selection is entered.
    resetMatch();mode=0;TestScene q4Scene(1536);testScenes={&q4Scene};
    testGlobal(0xd9f3970,1165);testGlobal(0xd9f3974,1158);
    world.phase(6);testPut(reinterpret_cast<A>(world.manager.data())+8,0);
    testPut(world.wrapper[2]+0x1d8,0);testPut(world.wrapper[2]+0x1dc,0);
    testPut(reinterpret_cast<A>(world.clock.data())+0x28,900.0f);
    lastDecision=0;assert(guardedDecision());lastDecision=0;assert(guardedDecision());
    world.phase(5);testPut(reinterpret_cast<A>(world.manager.data())+8,4);
    testPut(world.wrapper[2]+0x1d8,14);testPut(world.wrapper[2]+0x1dc,14);
    lastDecision=0;assert(guardedDecision());int q4Opening=activeLoss;
    assert(!seatPatternPending);checkScene(q4Scene,q4Opening);
    world.phase(1);testPut(world.wrapper[2]+0x1dc,49);
    testPut(reinterpret_cast<A>(world.clock.data())+0x28,800.0f);lastDecision=0;assert(guardedDecision());
    for(float clock:{700.0f,600.0f,500.0f}){
        testPut(reinterpret_cast<A>(world.clock.data())+0x28,clock);lastDecision=0;assert(guardedDecision());
        assert(activeLoss==q4Opening);checkScene(q4Scene,q4Opening);
    }
    assert(departureModel.catchupQueued);world.phase(5);lastDecision=0;assert(guardedDecision());
    int bounded=(std::min)(departureModel.plannedLoss,departureModel.periodStartLoss+settings->quarterCap);
    assert(activeLoss>q4Opening&&activeLoss==bounded&&activeLoss-q4Opening<=settings->quarterCap);
    int caughtUp=activeLoss;checkScene(q4Scene,activeLoss);
    assert(!departureModel.catchupQueued); // The accepted pressure was settled once.
    lastDecision=0;assert(guardedDecision());assert(activeLoss==caughtUp);
    guardedQuarterBreak(3,4);assert(activeLoss==caughtUp); // A duplicate same-time checkpoint cannot grant another visible batch.
    mode=2;assert(guardedApply(false,true));
    for(auto p:worldPages)VirtualFree(p,0,MEM_RELEASE);VirtualFree(allocation,0,MEM_RELEASE);
    std::cout<<"PASS: production capture/visibility, cached layouts, arrivals/replays, late intro renderers, kickoff/Midgame Save opening preservation, quit/reload, phase0/5 Super Sim Q2 departures, early halftime breaks/Q3 returns, Q4 home/FCS exits, pause isolation and live-play gating.\n";
}
