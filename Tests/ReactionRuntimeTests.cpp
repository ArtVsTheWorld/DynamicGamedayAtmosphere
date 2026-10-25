#define WIN32_LEAN_AND_MEAN
#include "../Source/ReactionAudioRuntime.h"
#include <cassert>
#include <cstdio>
using namespace attendance::sustained;
constexpr Address image=0x140000000;
unsigned char entity[0x200]{},asset[0x80]{},systemData[0x60]{},list[0x20]{},description[0x60]{},reactionValue[0x60]{},parameter[0x60]{},instances[16]{};
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
void __fastcall set(Address v,int n){assert(n==91);put(v,n);}
Address __fastcall receive(Address target,Address){
    assert(target==addr(entity)+0x40&&at<int>(addr(reactionValue))==91);++sends;
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
    std::puts("PASS: tested native size-3 cheer, event cleanup, once-per-eruption ownership, native duplicate suppression, leases/expiry, reentrancy, asset validation and dispatch faults.");
}
