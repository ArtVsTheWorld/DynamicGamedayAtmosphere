#pragma once
#include <windows.h>
#include <atomic>
#include <cstring>
#include <mutex>
#include "SustainedCrowd.h"
#include "GameBindings.h"

namespace attendance::sustained {
using Address=uintptr_t;
struct Api {
    using Query=Address(__fastcall*)(Address,Address,Address,Address,Address);
    using Get=Address(__fastcall*)(Address,bool);
    using Destroy=void(__fastcall*)(Address);
    using Weak=void(__fastcall*)(Address,Address*);
    using Lookup=Address(__fastcall*)(Address,Address,const char*,const char*,Address*);
    using Construct=Address(__fastcall*)(Address,Address,Address);
    using Set=void(__fastcall*)(Address,int);
    using Send=Address(__fastcall*)(Address,Address,Address);
    using Init=Address(__fastcall*)(Address);
    using ManagerGet=Address(__fastcall*)(Address);
    Query query=nullptr;Get get=nullptr;Destroy queryDestroy=nullptr;Weak weakRelease=nullptr;
    Lookup lookup=nullptr;Construct construct=nullptr;Destroy eventDestroy=nullptr;Set set=nullptr;Send send=nullptr;
    Init arenaInit=nullptr;Destroy arenaDestroy=nullptr;
    ManagerGet managerGet=nullptr;
    static Api native(Address image){
        Api a;a.query=reinterpret_cast<Query>(image+bindings::SwellQuery);a.get=reinterpret_cast<Get>(image+bindings::SwellQueryGet);
        a.queryDestroy=reinterpret_cast<Destroy>(image+bindings::SwellQueryDestroy);a.weakRelease=reinterpret_cast<Weak>(image+bindings::SwellWeakRelease);
        a.lookup=reinterpret_cast<Lookup>(image+bindings::SwellLookup);a.construct=reinterpret_cast<Construct>(image+bindings::SwellConstruct);
        a.eventDestroy=reinterpret_cast<Destroy>(image+bindings::SwellDestroy);a.set=reinterpret_cast<Set>(image+bindings::SwellSetInt);
        a.send=reinterpret_cast<Send>(image+bindings::SwellSend);a.arenaInit=reinterpret_cast<Init>(image+bindings::SwellArenaInit);
        a.arenaDestroy=reinterpret_cast<Destroy>(image+bindings::SwellArenaDestroy);
        a.managerGet=reinterpret_cast<ManagerGet>(image+bindings::SwellManagerGetter);return a;
    }
};
template<class T>T at(Address p){T value{};SIZE_T copied=0;if(p>=0x10000)ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),&value,sizeof(value),&copied);return copied==sizeof(value)?value:T{};}
inline bool named(Address p,const char* name){
    char value[32]{};SIZE_T copied=0;size_t length=std::strlen(name)+1;
    return p>=0x10000&&length<=sizeof(value)&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),value,length,&copied)&&copied==length&&!std::memcmp(value,name,length);
}
// Leaf wrappers keep native structured exceptions inside this optional feature;
// owning C++ scopes still run their cleanup after a failed engine call.
template<class Function,class... Args>bool invoke(Function fn,Args... args){
    __try{fn(args...);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class Function,class... Args>bool obtain(Address& result,Function fn,Args... args){
    __try{result=fn(args...);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
#pragma warning(push)
#pragma warning(disable:4324)
class Endpoint {
    const Api& api_;bool queryReady_=false;
    alignas(32) unsigned char query_[0x100]{};Address weak_=0;
public:
    Address manager=0,world=0,asset=0;
    explicit Endpoint(const Api& api):api_(api){}
    Endpoint(const Endpoint&)=delete;Endpoint& operator=(const Endpoint&)=delete;
    ~Endpoint(){if(weak_)invoke(api_.weakRelease,weak_,&weak_);if(queryReady_)invoke(api_.queryDestroy,reinterpret_cast<Address>(query_));}
    bool resolve(Address image,Address owner){
        if(owner<0x10040||at<Address>(owner-0x40)!=image+0xb220ce8||at<Address>(owner)!=image+0xb220dc8||at<Address>(owner+8)!=image+0xb220dd8)return false;
        Address entity=owner-0x40,allocator=at<Address>(image+bindings::SwellAllocator);
        world=at<Address>(entity+0x30);asset=at<Address>(entity+0x38);if(!allocator||!world||!asset)return false;
        struct Target {Address asset;unsigned hash;unsigned char flag,padding[3];} target{asset,0x243a8b25,0,{}};
        Address unused=0,q=reinterpret_cast<Address>(query_);
        if(!obtain(unused,api_.query,q,allocator,reinterpret_cast<Address>(&target),image+bindings::SwellQueryType,world))return false;
        queryReady_=true;Address result=0;
        if(!obtain(result,api_.get,q,false)||!result)return false;
        auto weakGetter=reinterpret_cast<void(__fastcall*)(Address,Address*)>(at<Address>(at<Address>(result)+0x60));
        if(!weakGetter||!invoke(weakGetter,result,&weak_))return false;
        Address endpoint=at<Address>(weak_);if(endpoint<8)return false;endpoint-=8;
        Address getter=at<Address>(at<Address>(endpoint)+0xe0);
        if(getter!=image+bindings::SwellManagerGetter)return false;
        return obtain(manager,api_.managerGet,endpoint)&&manager!=0;
    }
};
#pragma warning(pop)
inline bool emit(const Api& api,Address image,Address manager,bool start,int size=3,bool* dispatchAttempted=nullptr,bool* notReady=nullptr,int attackMs=4000,int releaseMs=4000){
    if(dispatchAttempted)*dispatchAttempted=false;
    if(notReady)*notReady=false;
    alignas(32) unsigned char result[64]{},event[0xb0]{},arena[0x48]{};Address description=0,unused=0;
    const char* eventName=start?"Trigger_AreaPrePlaySwell":"Stop_AreaPrePlaySwell";
    if(!obtain(unused,api.lookup,manager,reinterpret_cast<Address>(result),"CORE_BG_CRWD_EventSystem",eventName,&description))return false;
    if(!description){if(notReady)*notReady=true;return false;}
    if(at<unsigned>(description+0x40)!=205964879||at<unsigned>(description+0x44)!=(start?1690946487u:2466082807u))return false;
    *reinterpret_cast<Address*>(arena)=image+bindings::SwellArenaVtable;
    *reinterpret_cast<unsigned*>(arena+8)=1;*reinterpret_cast<unsigned*>(arena+12)=1;
    if(!obtain(unused,api.arenaInit,reinterpret_cast<Address>(arena+0x18)))return false;
    struct Cleanup {
        const Api& api;Address arena,event;bool constructed=false;
        ~Cleanup(){if(constructed)invoke(api.eventDestroy,event);invoke(api.arenaDestroy,arena);}
    } cleanup{api,reinterpret_cast<Address>(arena),reinterpret_cast<Address>(event)};
    if(!obtain(unused,api.construct,cleanup.event,cleanup.arena,description))return false;
    cleanup.constructed=true;
    unsigned count=at<unsigned>(cleanup.event+0x18);
    if(at<unsigned char>(cleanup.event+0xac)!=1||count!=(start?2u:1u))return false;
    Address entries=at<Address>(cleanup.event+0x20);unsigned seen=0;
    for(unsigned i=0;i<count;++i){
        Address value=at<Address>(entries+i*sizeof(Address)),desc=at<Address>(value+0x30),label=at<Address>(desc+0x20);
        if(at<int>(desc+0x40)!=1||at<unsigned char>(desc+0x44)!=0)return false;
        unsigned bit=0;int requested=4000;
        if(start&&named(label,"reacSize")){bit=1;requested=(std::clamp)(size,1,3);}
        else if(start&&named(label,"swellAttack")){bit=2;requested=(std::clamp)(attackMs,500,4000);}
        else if(!start&&named(label,"swellRelease")){bit=4;requested=(std::clamp)(releaseMs,500,30000);}
        if(!bit||(seen&bit)||!invoke(api.set,value,requested))return false;
        seen|=bit;
    }
    if(seen!=(start?3u:4u))return false;
    if(dispatchAttempted)*dispatchAttempted=true;
    return obtain(unused,api.send,manager,reinterpret_cast<Address>(result),cleanup.event);
}
using Note=void(*)(const char*);
class Runtime {
    Address image_=0,manager_=0,world_=0,asset_=0,owner_=0;Api api_{};Note note_=nullptr;
    Episode episode_;std::mutex mutex_;uint64_t lastResolve_=0,retryUntil_=0,lastRefresh_=0;unsigned failures_=0;
    std::atomic<bool> healthy_{true},playing_{false};std::atomic<unsigned> starts_{0},stops_{0};
    std::atomic<int> reason_{0};
    std::atomic<Address> openingManager_{0};std::atomic<unsigned> nativeStops_{0},refreshes_{0};unsigned handledStop_=0;int releaseMs_=4000;
public:
    void configure(Address image,Note note=nullptr,const Api* api=nullptr){image_=image;api_=api?*api:Api::native(image);note_=note;}
    void tick(Address owner,Intent wanted,uint64_t now){
        static thread_local bool executing=false;if(executing)return;
        struct Guard {bool& flag;explicit Guard(bool& value):flag(value){flag=true;}~Guard(){flag=false;}} executingGuard(executing);
        std::lock_guard<std::mutex> lock(mutex_);
        if(!healthy_&&!episode_.playing)return;
        if(!fresh(wanted,wanted.match,wanted.epoch,now))wanted.active=false;
        if(episode_.playing&&wanted.active&&(wanted.priority>episode_.priority||(episode_.continuous&&wanted.continuous))){
            episode_.reason=wanted.reason;episode_.priority=wanted.priority;releaseMs_=(std::clamp)(wanted.releaseMs,500,30000);reason_=static_cast<int>(wanted.reason);
        }
        bool stop=episode_.mustStop(wanted,now)||(!healthy_&&episode_.playing);
        bool refresh=episode_.playing&&episode_.continuous&&nativeStops_.load()!=handledStop_&&now>=lastRefresh_&&now-lastRefresh_>=2000;
        if(!stop&&!refresh&&!episode_.canStart(wanted,now))return;
        if(now<retryUntil_||now-lastResolve_<100)return;lastResolve_=now;
        Endpoint endpoint(api_);
        if(!endpoint.resolve(image_,owner)){retryUntil_=now+1000;return;}
        if(episode_.playing&&(endpoint.manager!=manager_||endpoint.world!=world_||endpoint.asset!=asset_||owner!=owner_)){
            // Retired worlds own their retired voices. Never send through a
            // replacement endpoint to stop a sound belonging to another world.
            episode_.stopped(now);playing_=false;reason_=0;openingManager_=0;
            if(note_)note_("Anticipation released after the crowd scene changed.");return;
        }
        if(stop){
            int release=wanted.active&&healthy_?releaseMs_:4000;
            if(!emit(api_,image_,endpoint.manager,false,3,nullptr,nullptr,4000,release)){retryUntil_=now+1000;healthy_=false;if(note_)note_("Anticipation stop failed; no further swell starts will be sent.");return;}
            ++stops_;episode_.stopped(now,static_cast<unsigned>(release));playing_=false;reason_=0;openingManager_=0;
            if(note_)note_("Anticipation roar fading out; normal game reactions continue.");return;
        }
        if(refresh){
            handledStop_=nativeStops_.load();
            // Stock stop messages still run normally. Reassert the same native
            // looping channel during opening hype, before its release completes.
            bool attempted=false;
            if(!emit(api_,image_,endpoint.manager,true,3,&attempted,nullptr,500)){
                healthy_=false;retryUntil_=now+1000;return;
            }
            lastRefresh_=now;++refreshes_;return;
        }
        if(episode_.canStart(wanted,now)){
            bool attempted=false,notReady=false;
            bool sent=emit(api_,image_,endpoint.manager,true,wanted.size,&attempted,&notReady);
            if(!sent){
                retryUntil_=now+2000;
                if(attempted||(!notReady&&++failures_>=3)){healthy_=false;if(note_)note_("Anticipation start unavailable; further starts disabled for this session.");}
                if(!attempted)return;
            }
            episode_.started(wanted,now);releaseMs_=(std::clamp)(wanted.releaseMs,500,30000);manager_=endpoint.manager;world_=endpoint.world;asset_=endpoint.asset;owner_=owner;
            playing_=true;reason_=static_cast<int>(wanted.reason);++starts_;
            handledStop_=nativeStops_.load();openingManager_=wanted.continuous?manager_:0;
            if(sent&&note_)note_(name(wanted.reason));
        }
    }
    bool playing()const{return playing_;}bool healthy()const{return healthy_;}
    Reason reason()const{return static_cast<Reason>(reason_.load());}
    unsigned starts()const{return starts_;}unsigned stops()const{return stops_;}
    unsigned refreshes()const{return refreshes_;}
    void observeNativeStop(Address manager,Address event){
        // This runs inside the native send callback. Atomics only: the send
        // path may be nested under our audio mutex. No borrowed event survives.
        if(!manager||openingManager_.load()!=manager||at<unsigned char>(event+0xac)!=1)return;
        Address description=at<Address>(event+0xa0);
        if(at<unsigned>(description+0x40)==205964879&&at<unsigned>(description+0x44)==2466082807)++nativeStops_;
    }
    void fail(){healthy_=false;}
};
}
