#pragma once
#include "SwellAudioRuntime.h"

namespace attendance::cheer {
using Address=uintptr_t;
using Receiver=Address(__fastcall*)(Address,Address);
enum class Source {Live,Lineup,Result,Eruption};
enum class Delivery {Sent,NativePlaying,VoiceLimit,Failed,Expired,Cooldown};
using Note=void(*)(Source,uint64_t,Delivery);
// Eruption, conversion and defensive-stop families passed listening tests. Their native
// owner limits each reaction to 25 seconds; no borrowed entity is retained.
inline bool emit(const sustained::Api& api,Receiver receiver,Address image,Address entity,int type=91){
    using namespace sustained;
    if(type!=91&&type!=75&&type!=83&&type!=59&&type!=95)return false;
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
    if(at<int>(d+0x40)!=1||at<unsigned char>(d+0x44)||!named(at<Address>(d+0x20),"reacType")||!invoke(api.set,value,type))return false;
    return obtain(unused,receiver,entity+0x40,event);
}
class Runtime {
    Address image_=0,match_=0;unsigned epoch_=0;uint64_t consumed_=0,result_=0;
    uint64_t lineup_=0,lastLineupCheer_=0;
    sustained::Api api_;Receiver receiver_=nullptr;std::mutex mutex_;
    std::atomic<bool> healthy_{true};std::atomic<unsigned> sent_{0};
    uint64_t liveConsumed_=0;std::atomic<uint64_t> liveDelivered_{0},liveResolved_{0};Note note_=nullptr;
    void report(Source source,uint64_t id,Delivery delivery){
        if(source==Source::Live)liveResolved_=id;
        if(note_)note_(source,id,delivery);
    }
public:
    void configure(Address image,const sustained::Api* api=nullptr,Receiver receiver=nullptr,Note note=nullptr){
        image_=image;api_=api?*api:sustained::Api::native(image);
        receiver_=receiver?receiver:reinterpret_cast<Receiver>(image+bindings::ReactionReceive);note_=note;
    }
    void tick(Address entity,const sustained::Intent& wanted,uint64_t now){
        using namespace sustained;
        static thread_local bool executing=false;if(executing)return;
        struct Guard {bool& value;explicit Guard(bool& v):value(v){value=true;}~Guard(){value=false;}} guard(executing);
        if(!healthy_||!fresh(wanted,wanted.match,wanted.epoch,now))return;
        std::lock_guard<std::mutex> lock(mutex_);
        if(match_!=wanted.match||epoch_!=wanted.epoch){match_=wanted.match;epoch_=wanted.epoch;consumed_=result_=lineup_=lastLineupCheer_=liveConsumed_=0;liveDelivered_=liveResolved_=0;}
        bool victory=wanted.celebration&&now<wanted.celebrationUntil&&consumed_!=wanted.celebration;
        bool during=!wanted.celebration&&wanted.liveCheer&&now<wanted.liveCheerUntil&&liveConsumed_!=wanted.liveCheer;
        if(during){
            if(wanted.liveType!=83&&wanted.liveType!=59&&wanted.liveType!=95&&wanted.liveType!=91)return;
            if(at<Address>(entity)!=image_+bindings::ReactionVtable)return;
            Address asset=at<Address>(entity+0x38);if(at<int>(asset+0x40)!=3||at<float>(asset+0x44)!=25)return;
            Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128);
            if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;return;}
            unsigned positives=0;
            for(Address p=begin;p<end;p+=16){int active=at<int>(p);
                if(active==wanted.liveType){liveConsumed_=wanted.liveCheer;liveDelivered_=wanted.liveCheer;report(Source::Live,wanted.liveCheer,Delivery::NativePlaying);return;}
                if(active==91||active==75||active==83||active==59||active==95)++positives;
            }
            // Existing native reactions take the available space. Resolve this
            // opportunity now rather than adding a late cheer when one ends.
            if(positives>=2){liveConsumed_=wanted.liveCheer;report(Source::Live,wanted.liveCheer,Delivery::VoiceLimit);return;}
            liveConsumed_=wanted.liveCheer;
            if(emit(api_,receiver_,image_,entity,wanted.liveType)){++sent_;liveDelivered_=wanted.liveCheer;lastLineupCheer_=now;report(Source::Live,wanted.liveCheer,Delivery::Sent);}
            else{healthy_=false;report(Source::Live,wanted.liveCheer,Delivery::Failed);}
            return;
        }
        bool result=!wanted.celebration&&wanted.resultCheer&&now<wanted.resultCheerUntil&&result_!=wanted.resultCheer;
        bool lineup=false;
        if(!victory&&!result){
            if(!wanted.cheers||!wanted.lineupCheer||wanted.lineupCheer==lineup_||now<wanted.lineupCheerBegan)return;
            uint64_t age=now-wanted.lineupCheerBegan;
            // One early opportunity, never a delayed cheer after waiting at the
            // line. A result accent or native cheer can replace this burst.
            if(now>=wanted.lineupCheerUntil||age>=1500){lineup_=wanted.lineupCheer;report(Source::Lineup,lineup_,Delivery::Expired);return;}
            if(!wanted.hostileOpening&&lastLineupCheer_&&now>=lastLineupCheer_&&now-lastLineupCheer_<6000){lineup_=wanted.lineupCheer;report(Source::Lineup,lineup_,Delivery::Cooldown);return;}
            lineup=true;
        }
        if(at<Address>(entity)!=image_+bindings::ReactionVtable)return;
        Address asset=at<Address>(entity+0x38);
        if(at<int>(asset+0x40)!=3||at<float>(asset+0x44)!=25)return;
        // Consume before dispatch, including uncertain native failures. A score
        // may cross several entity updates/scenes but receives one bounded burst.
        if(victory)consumed_=wanted.celebration;
        else if(result){result_=wanted.resultCheer;lastLineupCheer_=now;}
        else{lineup_=wanted.lineupCheer;lastLineupCheer_=now;}
        unsigned sentNow=0,nativeNow=0;bool limited=false;
        auto send=[&](int type){
            // Re-read after each send: the native instance array may reallocate.
            Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128);
            if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;return;}
            unsigned positives=0;
            for(Address p=begin;p<end;p+=16){int active=at<int>(p);if(active==type){++nativeNow;return;}if(active==91||active==75||active==83||active==59||active==95)++positives;}
            if(positives>=2){limited=true;return;}
            if(emit(api_,receiver_,image_,entity,type)){++sent_;++sentNow;}else healthy_=false;
        };
        send(result?wanted.resultType:lineup?wanted.lineupType:91);
        // Hit together. Sustained roar/swell channels supply the aftermath;
        // no delayed or repeated cheer barrage follows the initial eruption.
        if((victory||lineup||result)&&wanted.deepCheers&&healthy_)send(75);
        report(victory?Source::Eruption:result?Source::Result:Source::Lineup,victory?wanted.celebration:result?wanted.resultCheer:wanted.lineupCheer,
            !healthy_?Delivery::Failed:sentNow?Delivery::Sent:nativeNow?Delivery::NativePlaying:limited?Delivery::VoiceLimit:Delivery::Failed);
    }
    void fail(){healthy_=false;}
    bool healthy()const{return healthy_;}unsigned sent()const{return sent_;}
    bool liveDelivered(uint64_t id)const{return id&&liveDelivered_.load()==id;}
    bool liveResolved(uint64_t id)const{return id&&liveResolved_.load()==id;}
};
}
