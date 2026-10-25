#define WIN32_LEAN_AND_MEAN
#include "../Source/SwellAudioRuntime.h"
#include <cassert>
#include <cstdio>
#include <vector>
using namespace attendance::sustained;
constexpr Address image=0x140000000;
template<class T>void put(Address p,T value){memcpy(reinterpret_cast<void*>(p),&value,sizeof(value));}
template<size_t N>Address address(unsigned char (&p)[N]){return reinterpret_cast<Address>(p);}
unsigned char owner[0x300]{},resultObject[0x80]{},resultVtable[0x100]{},weak[0x20]{},endpoint[0x100]{},endpointVtable[0x100]{};
unsigned char descriptions[2][0x60]{},values[2][0x50]{},parameters[2][0x60]{};Address entries[2]{};
Address manager=0x111111;bool startEvent=false,queryMissing=false,badParameter=false,faultSend=false,reenter=false;
unsigned queries=0,queryEnds=0,weaks=0,weakEnds=0,arenas=0,arenaEnds=0,events=0,eventEnds=0,starts=0,stops=0;
struct Message {bool start;int size,attack,release;};std::vector<Message> messages;
Runtime* recursive=nullptr;Intent request;uint64_t timeValue=1000;
Address __fastcall query(Address q,Address allocator,Address target,Address type,Address world){
    assert(q&&allocator==0x1234&&type==image+attendance::bindings::SwellQueryType&&world==0x2345);
    assert(at<unsigned>(target+8)==0x243a8b25);++queries;return q;
}
Address __fastcall get(Address,bool blocking){assert(!blocking);return queryMissing?0:address(resultObject);}
void __fastcall queryEnd(Address){++queryEnds;assert(weaks==weakEnds);}
void __fastcall weakGet(Address value,Address* out){assert(value==address(resultObject));*out=address(weak);++weaks;}
void __fastcall weakEnd(Address value,Address* out){assert(value==address(weak));*out=0;++weakEnds;}
Address __fastcall managerGet(Address value){assert(value==address(endpoint));return manager;}
Address __fastcall lookup(Address m,Address,const char* system,const char* name,Address* out){
    assert(m==manager&&!strcmp(system,"CORE_BG_CRWD_EventSystem"));startEvent=!strcmp(name,"Trigger_AreaPrePlaySwell");
    assert(startEvent||!strcmp(name,"Stop_AreaPrePlaySwell"));*out=address(descriptions[startEvent?0:1]);return 0;
}
Address __fastcall init(Address a){++arenas;assert(at<Address>(a-0x18)==image+attendance::bindings::SwellArenaVtable);return a;}
void __fastcall arenaEnd(Address){++arenaEnds;assert(events==eventEnds);}
Address __fastcall construct(Address e,Address,Address){
    ++events;put<unsigned char>(e+0xac,1);put<unsigned>(e+0x18,startEvent?2u:1u);put(e+0x20,reinterpret_cast<Address>(entries));
    for(unsigned i=0;i<(startEvent?2u:1u);++i){
        entries[i]=address(values[i]);put(entries[i]+0x30,address(parameters[i]));
        const char* label=badParameter?"unknown":startEvent?(i==0?"reacSize":"swellAttack"):"swellRelease";
        put(address(parameters[i])+0x20,reinterpret_cast<Address>(label));put<int>(address(parameters[i])+0x40,1);
    }return e;
}
void __fastcall eventEnd(Address){++eventEnds;}
void __fastcall set(Address v,int value){put(v,value);}
Address __fastcall send(Address m,Address,Address){
    assert(m==manager);Message message{startEvent,0,0,0};
    if(startEvent){++starts;message.size=at<int>(entries[0]);message.attack=at<int>(entries[1]);}
    else {++stops;message.release=at<int>(entries[0]);}
    messages.push_back(message);
    if(reenter&&recursive)recursive->tick(address(owner)+0x40,request,timeValue);
    if(faultSend){faultSend=false;RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);}return 0;
}
void balanced(){assert(queries==queryEnds&&weaks==weakEnds&&arenas==arenaEnds&&events==eventEnds);}
int main(){
    Address page=image+(attendance::bindings::SwellAllocator&~Address(0xffff));
    void* allocation=VirtualAlloc(reinterpret_cast<void*>(page),0x10000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);assert(reinterpret_cast<Address>(allocation)==page);
    put(image+attendance::bindings::SwellAllocator,Address(0x1234));
    put(address(owner),image+0xb220ce8);put(address(owner)+0x40,image+0xb220dc8);put(address(owner)+0x48,image+0xb220dd8);
    put(address(owner)+0x30,Address(0x2345));put(address(owner)+0x38,Address(0x3456));
    put(address(resultObject),address(resultVtable));put(address(resultVtable)+0x60,reinterpret_cast<Address>(weakGet));
    put(address(weak),address(endpoint)+8);put(address(endpoint),address(endpointVtable));put(address(endpointVtable)+0xe0,image+attendance::bindings::SwellManagerGetter);
    for(int i=0;i<2;++i){put<unsigned>(address(descriptions[i])+0x40,205964879);put<unsigned>(address(descriptions[i])+0x44,i?2466082807u:1690946487u);}
    Api api;api.query=query;api.get=get;api.queryDestroy=queryEnd;api.weakRelease=weakEnd;api.managerGet=managerGet;
    api.lookup=lookup;api.construct=construct;api.eventDestroy=eventEnd;api.set=set;api.send=send;api.arenaInit=init;api.arenaDestroy=arenaEnd;
    Runtime runtime;runtime.configure(image,nullptr,&api);recursive=&runtime;
    request={};request.match=123;request.epoch=1;request.updated=timeValue;request.active=true;request.reason=Reason::Opening;request.size=3;request.intensity=1000;
    auto tick=[&](uint64_t time){timeValue=time;request.updated=time;runtime.tick(address(owner)+0x40,request,time);balanced();};
    reenter=true;tick(1000);assert(runtime.playing()&&starts==1&&messages.back().size==3&&messages.back().attack==4000);
    reenter=false;
    for(uint64_t time=1100;time<21000;time+=100)tick(time);assert(starts==1&&queries==1); // No per-frame endpoint work while holding.
    tick(21000);assert(!runtime.playing()&&stops==1&&messages.back().release==4000);
    tick(28000);assert(starts==1);tick(29000);assert(starts==2);
    request.active=false;tick(29100);assert(stops==2&&!runtime.playing()); // Disable cleans up through the same live endpoint.
    request.active=true;tick(37100);assert(starts==3);runtime.tick(address(owner)+0x40,request,38000);balanced();assert(stops==3); // Expired lease.
    request.match=456;request.epoch=2;tick(46000);assert(starts==4);
    manager=0x222222;request.active=false;tick(46100);assert(stops==3&&!runtime.playing()); // Replacement manager never gets an old stop.
    request.active=true;queryMissing=true;tick(55000);assert(!runtime.playing());queryMissing=false;tick(56100);assert(runtime.playing());
    request.reaction=true;tick(56200);assert(!runtime.playing()&&stops==4);request.reaction=false;
    Runtime failed;failed.configure(image,nullptr,&api);badParameter=true;request.updated=65000;
    failed.tick(address(owner)+0x40,request,65000);balanced();assert(!failed.playing());badParameter=false;
    Runtime faulted;faulted.configure(image,nullptr,&api);faultSend=true;request.updated=68000;
    faulted.tick(address(owner)+0x40,request,68000);balanced();assert(!faulted.healthy()&&faulted.playing());
    request.updated=70100;faulted.tick(address(owner)+0x40,request,70100);balanced();assert(!faulted.playing()); // Attempted send faults still receive a guarded stop.
    Runtime continuous;continuous.configure(image,nullptr,&api);request.continuous=true;request.releaseMs=20000;request.reason=Reason::Opening;request.updated=100000;
    continuous.tick(address(owner)+0x40,request,100000);balanced();assert(continuous.playing());
    unsigned char stockStop[0xb0]{};put<unsigned char>(address(stockStop)+0xac,1);put(address(stockStop)+0xa0,address(descriptions[1]));
    continuous.observeNativeStop(manager+1,address(stockStop));request.updated=125000;
    continuous.tick(address(owner)+0x40,request,125000);balanced();assert(continuous.playing()&&continuous.refreshes()==0); // No 20-second opening cutoff.
    continuous.observeNativeStop(manager,address(stockStop));request.updated=125100;
    continuous.tick(address(owner)+0x40,request,125100);balanced();assert(continuous.refreshes()==1&&messages.back().start&&messages.back().attack==500);
    request.updated=140000;request.offense=1;continuous.tick(address(owner)+0x40,request,140000);balanced();assert(continuous.playing());
    request.continuous=false;request.reason=Reason::None;request.updated=140100;
    continuous.tick(address(owner)+0x40,request,140100);balanced();assert(!continuous.playing()&&!messages.back().start&&messages.back().release==20000);
    assert(VirtualFree(allocation,0,MEM_RELEASE));
    std::puts("PASS: native query/weak/event/arena cleanup, tested parameters, bounded playback, reentrancy, stale lease, disable, context replacement, unavailable assets and dispatch faults.");
}
