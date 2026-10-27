#define WIN32_LEAN_AND_MEAN
#include "../Source/ReactionAudioRuntime.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <set>
using namespace attendance::sustained;
constexpr Address image=0x140000000;
unsigned char entity[0x200]{},asset[0x80]{},systemData[0x60]{},list[0x20]{},description[0x60]{},reactionValue[0x60]{},parameter[0x60]{},instances[32]{};
std::vector<int> received;
std::vector<attendance::cheer::Delivery> reports;
void report(attendance::cheer::Source,uint64_t,attendance::cheer::Delivery delivery){reports.push_back(delivery);}
Address entries[1]{};unsigned arenas=0,arenaEnds=0,events=0,eventEnds=0,sends=0;
bool badParameter=false,fault=false,reenter=false;attendance::cheer::Runtime* recursive=nullptr;Intent wanted;
template<size_t N>Address addr(unsigned char (&p)[N]){return reinterpret_cast<Address>(p);}
template<class T>void put(Address p,T v){memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
Address __fastcall arenaInit(Address a){++arenas;assert(at<Address>(a-0x18)==image+attendance::bindings::SwellArenaVtable);return a;}
void __fastcall arenaEnd(Address){++arenaEnds;assert(events==eventEnds);}
Address __fastcall construct(Address e,Address,Address d){
    assert(d==addr(description));++events;put<unsigned char>(e+0xac,1);put<unsigned>(e+0x18,1);
    entries[0]=addr(reactionValue);put(e+0x20,reinterpret_cast<Address>(entries));put(addr(reactionValue)+0x30,addr(parameter));
    put<int>(addr(parameter)+0x40,1);put<unsigned char>(addr(parameter)+0x44,0);
    put(addr(parameter)+0x20,reinterpret_cast<Address>(badParameter?"wrong":"reacType"));return e;
}
void __fastcall eventEnd(Address){++eventEnds;}
void __fastcall set(Address v,int n){assert(n==91||n==75||n==83||n==59||n==95||n==11);put(v,n);}
Address __fastcall receive(Address target,Address){
    assert(target==addr(entity)+0x40);int type=at<int>(addr(reactionValue));assert(type==91||type==75||type==83||type==59||type==95||type==11);received.push_back(type);++sends;
    if(reenter&&recursive)recursive->tick(addr(entity),wanted,wanted.updated);
    if(fault){fault=false;RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);}return 0;
}
void balanced(){assert(arenas==arenaEnds&&events==eventEnds);}
int main(){
    put(addr(entity),image+attendance::bindings::ReactionVtable);put(addr(entity)+0x38,addr(asset));
    put(addr(asset)+0x40,3);put(addr(asset)+0x44,25.0f);put(addr(asset)+0x20,addr(systemData));
    put<unsigned>(addr(systemData)+0x38,205964879);put(addr(systemData)+0x20,addr(list)+8);
    put<unsigned>(addr(list)+4,1);put(addr(list)+8,addr(description));
    put<unsigned>(addr(description)+0x40,205964879);put<unsigned>(addr(description)+0x44,3424066396u);
    put(addr(description)+0x38,reinterpret_cast<Address>("Trigger_AreaReaction"));
    Api api;api.arenaInit=arenaInit;api.arenaDestroy=arenaEnd;api.construct=construct;api.eventDestroy=eventEnd;api.set=set;
    attendance::cheer::Runtime runtime;runtime.configure(image,&api,receive);recursive=&runtime;
    wanted.match=123;wanted.epoch=1;wanted.updated=1000;wanted.active=true;wanted.celebration=1;wanted.celebrationUntil=7000;
    reenter=true;runtime.tick(addr(entity),wanted,1000);balanced();assert(sends==1&&runtime.sent()==1);reenter=false;
    for(unsigned i=0;i<100;++i)runtime.tick(addr(entity),wanted,1000);assert(sends==1);
    wanted.celebration=2;wanted.active=false;runtime.tick(addr(entity),wanted,1000);assert(sends==1);
    wanted.active=true;runtime.tick(addr(entity),wanted,2000);assert(sends==1); // Stale lease.
    wanted.updated=7000;runtime.tick(addr(entity),wanted,7000);assert(sends==1); // Expired celebration.
    wanted.updated=2000;wanted.celebrationUntil=8000;
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),91);
    runtime.tick(addr(entity),wanted,2000);assert(sends==1); // Native cheer already playing; don't stack another.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);runtime.tick(addr(entity),wanted,2000);assert(sends==1);
    wanted.epoch=2;runtime.tick(addr(entity),wanted,2000);balanced();assert(sends==2);
    wanted.celebration=3;put(addr(asset)+0x44,30.0f);runtime.tick(addr(entity),wanted,2000);assert(sends==2);
    put(addr(asset)+0x44,25.0f);runtime.tick(addr(entity),wanted,2000);balanced();assert(sends==3);
    attendance::cheer::Runtime failed;failed.configure(image,&api,receive);badParameter=true;failed.tick(addr(entity),wanted,2000);
    balanced();assert(!failed.healthy()&&sends==3);badParameter=false;
    attendance::cheer::Runtime faulted;faulted.configure(image,&api,receive);fault=true;faulted.tick(addr(entity),wanted,2000);
    balanced();assert(!faulted.healthy()&&sends==4);faulted.tick(addr(entity),wanted,2000);assert(sends==4);
    attendance::cheer::Runtime lineup;lineup.configure(image,&api,receive);unsigned initial=sends;
    wanted={};wanted.active=true;wanted.match=999;wanted.epoch=4;wanted.lineupCheer=1;wanted.lineupCheerBegan=1000;wanted.lineupCheerUntil=5000;wanted.size=3;
    auto tick=[&](uint64_t now){wanted.updated=now;lineup.tick(addr(entity),wanted,now);balanced();};
    tick(999);assert(sends==initial);tick(1000);assert(sends==initial+1);
    for(uint64_t now=3600;now<=28000;now+=100)tick(now);assert(sends==initial+1); // No second late cheer.
    wanted.lineupCheer=2;wanted.lineupCheerBegan=30000;wanted.lineupCheerUntil=34000;wanted.cheers=false;tick(32500);assert(sends==initial+1);
    wanted.cheers=true;tick(34500);assert(sends==initial+1); // A snap/missed callback cannot backfill an expired burst.
    wanted.lineupCheer=3;wanted.lineupCheerBegan=40000;wanted.lineupCheerUntil=44000;wanted.active=false;tick(42500);assert(sends==initial+1);
    wanted.active=true;wanted.celebration=555;wanted.celebrationUntil=50000;tick(42501);assert(sends==initial+2);
    attendance::cheer::Runtime varied;varied.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=1001;wanted.epoch=1;wanted.lineupCheer=9;wanted.lineupCheerBegan=100000;wanted.lineupCheerUntil=104500;wanted.size=3;
    auto variedTick=[&](uint64_t now){wanted.updated=now;varied.tick(addr(entity),wanted,now);balanced();};
    variedTick(99999);assert(sends==initial);variedTick(100000);assert(sends==initial+1);
    variedTick(122000);assert(sends==initial+1);
    wanted.lineupCheer=10;wanted.lineupCheerBegan=104000;wanted.lineupCheerUntil=108500;variedTick(104000);assert(sends==initial+1); // Shared cooldown replaces a generic burst with the recent cheer.
    variedTick(110000);assert(sends==initial+1);
    attendance::cheer::Runtime burst;burst.configure(image,&api,receive);initial=sends;received.clear();
    wanted={};wanted.active=true;wanted.match=2000;wanted.epoch=1;wanted.updated=200000;wanted.celebration=1;wanted.celebrationUntil=206000;wanted.deepCheers=true;
    reenter=true;recursive=&burst;burst.tick(addr(entity),wanted,200000);reenter=false;
    assert(sends==initial+2&&received==std::vector<int>({91,75}));balanced();
    burst.tick(addr(entity),wanted,200000);assert(sends==initial+2); // Both layers belong to one burst, never a delayed barrage.
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),91);
    wanted.celebration=2;burst.tick(addr(entity),wanted,200000);assert(sends==initial+3&&received.back()==75);balanced();
    put<int>(addr(instances),75);wanted.celebration=3;burst.tick(addr(entity),wanted,200000);assert(sends==initial+4&&received.back()==91);balanced();
    put<int>(addr(instances),91);put<int>(addr(instances)+16,75);put(addr(entity)+0x128,addr(instances)+32);
    wanted.celebration=4;burst.tick(addr(entity),wanted,200000);assert(sends==initial+4); // Native copies count toward the two-family limit.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    attendance::cheer::Runtime critical;critical.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=2001;wanted.epoch=1;wanted.lineupCheer=1;wanted.lineupCheerBegan=300000;wanted.lineupCheerUntil=303500;wanted.size=3;wanted.deepCheers=true;
    auto criticalTick=[&](uint64_t now){wanted.updated=now;critical.tick(addr(entity),wanted,now);balanced();};
    criticalTick(299999);assert(sends==initial);criticalTick(300000);assert(sends==initial+2&&received.back()==75);
    criticalTick(309000);assert(sends==initial+2); // Both strong accents hit together near lineup, not later.
    wanted.cheers=false;wanted.deepCheers=false;wanted.resultCheer=1;wanted.resultCheerUntil=313000;criticalTick(311000);assert(sends==initial+3&&received.back()==91);
    criticalTick(311100);assert(sends==initial+3);wanted.resultCheer=2;criticalTick(313000);assert(sends==initial+3);
    wanted.deepCheers=true;wanted.celebration=1;wanted.celebrationUntil=320000;criticalTick(314000);assert(sends==initial+5); // Eruption takes priority over any pending result accent.
    // Use the tested sound family selected by verified gameplay, dedupe it,
    // and count native copies of every allowed family toward the voice limit.
    attendance::cheer::Runtime typed;typed.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=3000;wanted.epoch=1;wanted.updated=400000;wanted.resultCheer=1;wanted.resultCheerUntil=403000;wanted.resultType=83;
    typed.tick(addr(entity),wanted,400000);assert(sends==initial+1&&received.back()==83);balanced();
    typed.tick(addr(entity),wanted,400000);assert(sends==initial+1);
    wanted.resultCheer=2;wanted.resultType=59;typed.tick(addr(entity),wanted,400000);assert(sends==initial+2&&received.back()==59);balanced();
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+32);put<int>(addr(instances),83);put<int>(addr(instances)+16,59);
    wanted.celebration=1;wanted.celebrationUntil=406000;wanted.deepCheers=true;typed.tick(addr(entity),wanted,400000);assert(sends==initial+2);
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    wanted.celebration=0;wanted.resultCheer=3;wanted.resultType=59;wanted.deepCheers=true;typed.tick(addr(entity),wanted,400000);
    assert(sends==initial+4&&received[received.size()-2]==59&&received.back()==75);balanced();
    typed.tick(addr(entity),wanted,400000);assert(sends==initial+4); // One layered result, not a repeated cheer sequence.
    attendance::cheer::Runtime early;early.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=4000;wanted.epoch=1;wanted.lineupCheer=1;wanted.lineupCheerBegan=500000;wanted.lineupCheerUntil=501850;wanted.deepCheers=true;
    auto earlyTick=[&](uint64_t now){wanted.updated=now;early.tick(addr(entity),wanted,now);balanced();};
    earlyTick(499999);assert(sends==initial);earlyTick(500000);assert(sends==initial+2);
    for(uint64_t now=500400;now<=540000;now+=100)earlyTick(now);assert(sends==initial+2); // No additional late cheer during the longer roar.
    wanted.lineupCheer=2;wanted.lineupCheerBegan=550000;wanted.lineupCheerUntil=551850;wanted.cheers=false;earlyTick(550350);assert(sends==initial+2);
    wanted.cheers=true;earlyTick(552000);assert(sends==initial+2); // A quick snap cannot create a delayed lineup cheer.
    for(int unverified:{87,99,107})assert(!attendance::cheer::emit(api,receive,image,addr(entity),unverified));balanced();
    attendance::cheer::Runtime earlyLive;earlyLive.configure(image,&api,receive,report);unsigned liveBefore=sends;
    wanted={};wanted.active=true;wanted.match=990;wanted.epoch=1;wanted.updated=600000;wanted.liveCheer=1;wanted.liveCheerUntil=600500;wanted.liveType=83;
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    earlyLive.tick(addr(entity),wanted,600000);balanced();assert(sends==liveBefore+1&&received.back()==83&&earlyLive.liveDelivered(1)&&earlyLive.liveResolved(1)&&reports.back()==attendance::cheer::Delivery::Sent);
    earlyLive.tick(addr(entity),wanted,600100);assert(sends==liveBefore+1);
    wanted.liveCheer=2;wanted.liveType=59;wanted.liveCheerUntil=600700;wanted.updated=600200;
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+32);put<int>(addr(instances),91);put<int>(addr(instances)+16,75);
    earlyLive.tick(addr(entity),wanted,600200);assert(sends==liveBefore+1&&!earlyLive.liveDelivered(2)&&earlyLive.liveResolved(2)&&reports.back()==attendance::cheer::Delivery::VoiceLimit);
    put<Address>(addr(entity)+0x128,addr(instances)+16);wanted.updated=600300;earlyLive.tick(addr(entity),wanted,600300);balanced();assert(sends==liveBefore+1&&!earlyLive.liveDelivered(2));
    wanted.liveCheer=3;wanted.updated=600800;earlyLive.tick(addr(entity),wanted,600800);assert(sends==liveBefore+1); // No delayed post-whistle cheer.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    attendance::cheer::Runtime disappointment;disappointment.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=991;wanted.epoch=1;wanted.updated=700000;wanted.liveCheer=1;wanted.liveCheerUntil=700500;wanted.liveType=95;
    disappointment.tick(addr(entity),wanted,700000);balanced();assert(sends==initial+1&&received.back()==95&&disappointment.liveDelivered(1));
    disappointment.tick(addr(entity),wanted,700100);assert(sends==initial+1);
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),95);
    wanted.liveCheer=2;disappointment.tick(addr(entity),wanted,700100);assert(sends==initial+1&&disappointment.liveDelivered(2)); // Native letdown is not duplicated.
    put<int>(addr(instances)+16,59);put(addr(entity)+0x128,addr(instances)+32);wanted.liveCheer=3;wanted.liveType=83;
    disappointment.tick(addr(entity),wanted,700200);assert(sends==initial+1); // Negative families share the same two-voice limit.
    // A 40+ yard completion uses the tested large cheer on the SAME live
    // delivery path, with identical duplicate and two-voice protections.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    attendance::cheer::Runtime explosive;explosive.configure(image,&api,receive,report);initial=sends;
    wanted={};wanted.active=true;wanted.match=992;wanted.epoch=1;wanted.updated=800000;wanted.liveCheer=1;wanted.liveCheerUntil=800500;wanted.liveType=91;
    explosive.tick(addr(entity),wanted,800000);assert(sends==initial+1&&received.back()==91&&explosive.liveDelivered(1));balanced();
    explosive.tick(addr(entity),wanted,800100);assert(sends==initial+1);
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),91);
    wanted.liveCheer=2;explosive.tick(addr(entity),wanted,800100);assert(sends==initial+1&&explosive.liveDelivered(2));
    put<int>(addr(instances),83);put<int>(addr(instances)+16,59);put(addr(entity)+0x128,addr(instances)+32);wanted.liveCheer=3;
    explosive.tick(addr(entity),wanted,800100);assert(sends==initial+1&&explosive.liveResolved(3)&&!explosive.liveDelivered(3));
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    attendance::cheer::Runtime hostile;hostile.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=993;wanted.epoch=1;wanted.updated=900000;wanted.lineupCheer=1;wanted.lineupCheerBegan=900000;wanted.lineupCheerUntil=901500;
    wanted.cheers=wanted.deepCheers=wanted.hostileOpening=true;wanted.lineupType=59;
    hostile.tick(addr(entity),wanted,900000);assert(sends==initial+2&&received[received.size()-2]==59&&received.back()==75);balanced();
    hostile.tick(addr(entity),wanted,900100);assert(sends==initial+2); // No delayed or repeated opening accents.
    wanted.lineupCheer=2;wanted.updated=wanted.lineupCheerBegan=900200;wanted.lineupCheerUntil=901700;
    hostile.tick(addr(entity),wanted,900200);assert(sends==initial+4); // Opener is not suppressed by the kickoff's recent cheer.
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+32);put<int>(addr(instances),83);put<int>(addr(instances)+16,91);
    wanted.lineupCheer=3;hostile.tick(addr(entity),wanted,900200);assert(sends==initial+4); // Existing two-voice limit remains authoritative.
    // The second cheer may overlap a matching native family AFTER the gap;
    // it never keeps retrying when the two-voice budget is full.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    attendance::cheer::Runtime stagger;stagger.configure(image,&api,receive,report);initial=sends;
    wanted={};wanted.active=true;wanted.staggered=true;wanted.playSequence=10;wanted.match=777;wanted.epoch=1;
    wanted.reason=Reason::Result;wanted.reactionId=10;wanted.majorLive=true;wanted.liveCheer=1;wanted.liveCheerUntil=1000500;wanted.liveType=83;
    auto delayed=[&](uint64_t now){wanted.updated=now;stagger.tick(addr(entity),wanted,now);balanced();};
    delayed(1000000);assert(sends==initial+1);
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),83);
    delayed(1000899);assert(sends==initial+1);delayed(1000900);assert(sends==initial+2&&received.back()==83);
    delayed(1001000);delayed(1001500);assert(sends==initial+2);
    // Matching native primary still permits the one scheduled second accent.
    wanted.liveCheer=2;wanted.liveCheerUntil=1002500;delayed(1002000);assert(sends==initial+2);
    delayed(1002900);assert(sends==initial+3);
    wanted.liveCheer=3;wanted.liveCheerUntil=1004500;delayed(1004000);
    put<int>(addr(instances)+16,59);put(addr(entity)+0x128,addr(instances)+32);delayed(1004900);assert(sends==initial+3);
    put(addr(entity)+0x128,addr(instances)+16);delayed(1005000);assert(sends==initial+3); // No retry after voice-limit skip.
    wanted.liveCheer=4;wanted.liveCheerUntil=1006500;delayed(1006000);wanted.playSequence=11;wanted.reactionId=11;delayed(1006900);assert(sends==initial+3);
    wanted.liveCheer=5;wanted.liveCheerUntil=1008500;delayed(1008000);wanted.active=false;delayed(1008100);wanted.active=true;delayed(1008900);assert(sends==initial+3); // Pause/replay clears pending.
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    wanted.liveCheer=0;wanted.celebration=99;wanted.celebrationUntil=1016000;wanted.deepCheers=true;delayed(1010000);assert(sends==initial+4&&received.back()==91);
    delayed(1010900);assert(sends==initial+5&&received.back()==75);delayed(1011000);assert(sends==initial+5);
    // Fourth-down conversion and a later long gain are separate live milestones.
    attendance::cheer::Runtime milestones;milestones.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.staggered=true;wanted.playSequence=40;wanted.match=778;wanted.epoch=1;wanted.liveType=83;wanted.liveCheer=1;wanted.liveCheerUntil=1020500;wanted.updated=1020000;
    milestones.tick(addr(entity),wanted,1020000);assert(sends==initial+1);
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),83);
    wanted.liveCheer=2;wanted.overlapMilestone=true;wanted.updated=1021200;wanted.liveCheerUntil=1021700;
    milestones.tick(addr(entity),wanted,1021200);assert(sends==initial+2&&received.back()==83);milestones.tick(addr(entity),wanted,1021300);assert(sends==initial+2);
    put<int>(addr(instances)+16,59);put(addr(entity)+0x128,addr(instances)+32);wanted.liveCheer=3;wanted.updated=1022400;wanted.liveCheerUntil=1022900;
    milestones.tick(addr(entity),wanted,1022400);assert(sends==initial+2); // Native two-voice cap still wins.
    // Trucks use the authored BIGHIT_POS large family, with native big-hit
    // instances included in the same two-voice budget as ordinary cheers.
    attendance::cheer::Runtime truck;truck.configure(image,&api,receive);initial=sends;
    put<Address>(addr(entity)+0x120,0);put<Address>(addr(entity)+0x128,0);
    wanted={};wanted.active=true;wanted.match=779;wanted.epoch=1;wanted.updated=1030000;wanted.liveType=11;wanted.liveCheer=1;wanted.liveCheerUntil=1030500;
    truck.tick(addr(entity),wanted,1030000);assert(sends==initial+1&&received.back()==11);balanced();
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),11);
    wanted.liveCheer=2;wanted.updated=1030100;truck.tick(addr(entity),wanted,1030100);assert(sends==initial+1);
    put<int>(addr(instances),8);put<int>(addr(instances)+16,15);put(addr(entity)+0x128,addr(instances)+32);
    wanted.liveCheer=3;wanted.updated=1030200;truck.tick(addr(entity),wanted,1030200);assert(sends==initial+1);
    // Offense and defense share positive full-size families, with stable
    // event choices and equivalent-family native duplicate suppression.
    for(int original:{59,83}){std::set<int> families;
        for(uint64_t play=1;play<=100;++play){int chosen=attendance::cheer::variedPositive(original,play,play+1,1);
            assert(chosen==59||chosen==83);families.insert(chosen);
            for(int callback=0;callback<20;++callback)assert(attendance::cheer::variedPositive(original,play,play+1,1)==chosen);
        }assert(families.size()==2);
    }
    for(int specific:{11,75,91,95})assert(attendance::cheer::variedPositive(specific,1,1,1)==specific);
    assert(attendance::cheer::variedPositive(59,0,1,1)==59);
    attendance::cheer::Runtime shared;shared.configure(image,&api,receive);initial=sends;
    wanted={};wanted.active=true;wanted.match=780;wanted.epoch=1;wanted.updated=1040000;wanted.liveCheer=1;wanted.liveCheerUntil=1040500;wanted.liveType=59;
    put(addr(entity)+0x120,addr(instances));put(addr(entity)+0x128,addr(instances)+16);put<int>(addr(instances),83);
    shared.tick(addr(entity),wanted,1040000);assert(sends==initial&&shared.liveDelivered(1));
    wanted.liveCheer=2;wanted.liveType=83;put<int>(addr(instances),59);shared.tick(addr(entity),wanted,1040100);assert(sends==initial&&shared.liveDelivered(2));
    std::puts("PASS: native duplicate handling, one bounded staggered same-family followup, native-inclusive voice limit/no retry, snap/pause cancellation, eruption priority, cleanup, reentrancy and faults.");
}
