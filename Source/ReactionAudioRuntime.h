#pragma once
#include "SwellAudioRuntime.h"

namespace attendance::cheer {
using Address=uintptr_t;
using Receiver=Address(__fastcall*)(Address,Address);
// The tested size-3 GEN_YEAH reaction. Its native owner limits playback to
// 25 seconds; no native instance or borrowed entity is retained by this code.
inline bool emit(const sustained::Api& api,Receiver receiver,Address image,Address entity){
    using namespace sustained;
    Address asset=at<Address>(entity+0x38),system=at<Address>(asset+0x20)&~Address(4);
    Address list=at<Address>(system+0x20),description=0;
    unsigned count=at<unsigned>(list-4)&0x7fffffff;
    if(at<unsigned>(system+0x38)!=205964879||!list||count>128)return false;
    for(unsigned i=0;i<count;++i){
        Address d=at<Address>(list+i*8)&~Address(4);
        if(at<unsigned>(d+0x40)==205964879&&at<unsigned>(d+0x44)==3424066396u&&named(at<Address>(d+0x38),"Trigger_AreaReaction")){description=d;break;}
    }
    if(!description)return false;
#pragma warning(push)
#pragma warning(disable:4324)
    struct Owned {
        const Api& api;alignas(32) unsigned char arena[0x48]{},event[0xb0]{};bool ar=false,ev=false;
        explicit Owned(const Api& a):api(a){}
        ~Owned(){if(ev)invoke(api.eventDestroy,reinterpret_cast<Address>(event));if(ar)invoke(api.arenaDestroy,reinterpret_cast<Address>(arena));}
    } owned(api);
#pragma warning(pop)
    *reinterpret_cast<Address*>(owned.arena)=image+bindings::SwellArenaVtable;
    *reinterpret_cast<unsigned*>(owned.arena+8)=1;*reinterpret_cast<unsigned*>(owned.arena+12)=1;
    Address unused=0;
    if(!obtain(unused,api.arenaInit,reinterpret_cast<Address>(owned.arena+0x18)))return false;owned.ar=true;
    if(!obtain(unused,api.construct,reinterpret_cast<Address>(owned.event),reinterpret_cast<Address>(owned.arena),description))return false;owned.ev=true;
    Address event=reinterpret_cast<Address>(owned.event);
    if(at<unsigned char>(event+0xac)!=1||at<unsigned>(event+0x18)!=1)return false;
    Address value=at<Address>(at<Address>(event+0x20)),d=at<Address>(value+0x30);
    if(at<int>(d+0x40)!=1||at<unsigned char>(d+0x44)||!named(at<Address>(d+0x20),"reacType")||!invoke(api.set,value,91))return false;
    return obtain(unused,receiver,entity+0x40,event);
}
class Runtime {
    Address image_=0,match_=0;unsigned epoch_=0;uint64_t consumed_=0;
    sustained::Api api_;Receiver receiver_=nullptr;std::mutex mutex_;
    std::atomic<bool> healthy_{true};std::atomic<unsigned> sent_{0};
public:
    void configure(Address image,const sustained::Api* api=nullptr,Receiver receiver=nullptr){
        image_=image;api_=api?*api:sustained::Api::native(image);
        receiver_=receiver?receiver:reinterpret_cast<Receiver>(image+bindings::ReactionReceive);
    }
    void tick(Address entity,const sustained::Intent& wanted,uint64_t now){
        using namespace sustained;
        static thread_local bool executing=false;if(executing)return;
        struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(executing);
        if(!healthy_||!fresh(wanted,wanted.match,wanted.epoch,now)||!wanted.celebration||now>=wanted.celebrationUntil)return;
        std::lock_guard<std::mutex> lock(mutex_);
        if(match_==wanted.match&&epoch_==wanted.epoch&&consumed_==wanted.celebration)return;
        if(at<Address>(entity)!=image_+bindings::ReactionVtable)return;
        Address asset=at<Address>(entity+0x38);
        if(at<int>(asset+0x40)!=3||at<float>(asset+0x44)!=25)return;
        Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128);
        if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;return;}
        // Consume before dispatch, including uncertain native failures. A score
        // may cross several entity updates/scenes but receives at most one cheer.
        match_=wanted.match;epoch_=wanted.epoch;consumed_=wanted.celebration;
        for(Address p=begin;p<end;p+=16)if(at<int>(p)==91)return; // The game already supplied this cheer.
        if(emit(api_,receiver_,image_,entity))++sent_;else healthy_=false;
    }
    void fail(){healthy_=false;}
    bool healthy()const{return healthy_;}unsigned sent()const{return sent_;}
};
}
