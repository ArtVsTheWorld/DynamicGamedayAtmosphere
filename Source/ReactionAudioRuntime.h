#pragma once
#include "SwellAudioRuntime.h"
#include <array>

namespace attendance::cheer {
using Address=uintptr_t;
using Receiver=Address(__fastcall*)(Address,Address);
enum class Source {Live,Lineup,Result,Eruption,Followup};
enum class Delivery {Sent,NativePlaying,VoiceLimit,Failed,Expired,Cooldown,Registered,Recovery,Unregistered};
using Note=void(*)(Source,uint64_t,Delivery);
inline bool countedVoice(int type){return type==91||type==75||type==83||type==59||type==95||(type>=8&&type<=15);}
// The two positive, full-size families may serve either side of the ball.
// Stable play/event identity prevents callback-by-callback rerolls. Dedicated
// disappointment, big-hit and scoring families keep their authored purpose.
inline int variedPositive(int type,uint64_t play,uint64_t event,unsigned epoch){
    if((type!=59&&type!=83)||!play||!event)return type;
    uint64_t mix=play*0x9e3779b97f4a7c15ull+event*0xbf58476d1ce4e5b9ull+epoch;
    mix^=mix>>30;mix*=0xbf58476d1ce4e5b9ull;mix^=mix>>27;
    return mix&1?59:83;
}
inline bool equivalentPositive(int requested,int active){return requested==active||((requested==59||requested==83)&&(active==59||active==83));}
// Eruption, conversion and defensive-stop families passed listening tests. Their native
// owner limits each reaction to 25 seconds; no borrowed entity is retained.
inline bool emit(const sustained::Api& api,Receiver receiver,Address image,Address entity,int type=91){
    using namespace sustained;
    if(type!=91&&type!=75&&type!=83&&type!=59&&type!=95&&type!=11)return false;
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
    uint64_t liveConsumed_=0,lastLiveAt_=0,lastLiveSequence_=0;std::atomic<uint64_t> liveDelivered_{0},liveResolved_{0};Note note_=nullptr;
    struct Followup {bool active=false;Source source=Source::Result;uint64_t id=0,sequence=0,due=0,until=0;int type=75;};
    Followup followup_;
    struct Receipt {bool active=false,retried=false;Address owner=0;Source source=Source::Live;uint64_t id=0,sequence=0,began=0,until=0;int type=0;unsigned observations=0;std::array<int,2> priorHandles{};unsigned priorCount=0;};
    Receipt receipt_;
    void receipt(Source source,uint64_t id,int type,Address entity,const sustained::Intent& wanted,uint64_t now){
        if(!wanted.playSequence||source==Source::Lineup)return;
        receipt_={true,false,entity,source,id,wanted.playSequence,now,now+600,type,0};
        auto begin=sustained::at<Address>(entity+0x120),end=sustained::at<Address>(entity+0x128);
        if(end>=begin&&end-begin<=256*16&&(end-begin)%16==0)for(Address p=begin;p<end&&receipt_.priorCount<2;p+=16)
            if(!sustained::at<unsigned char>(p+12)&&sustained::at<int>(p)==type)receipt_.priorHandles[receipt_.priorCount++]=sustained::at<int>(p+4);
    }
    void checkReceipt(Address entity,const sustained::Intent& wanted,uint64_t now){
        using namespace sustained;
        if(!receipt_.active)return;
        auto& r=receipt_;
        bool same=r.source==Source::Live?wanted.liveCheer==r.id:r.source==Source::Result?wanted.resultCheer==r.id:wanted.celebration==r.id;
        if(!same||r.sequence!=wanted.playSequence||r.owner!=entity||now<r.began){receipt_={};return;}
        Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128),queue=at<Address>(entity+0xd0),queueEnd=at<Address>(entity+0xd8);
        if(end<begin||end-begin>256*16||(end-begin)%16||queueEnd<queue||queueEnd-queue>256*8||(queueEnd-queue)%8){receipt_={};return;}
        unsigned voices=0;bool queued=false;
        for(Address p=begin;p<end;p+=16){if(at<unsigned char>(p+12))continue;int type=at<int>(p);
            bool existing=false;for(unsigned i=0;i<r.priorCount;++i)existing=existing||r.priorHandles[i]==at<int>(p+4);
            if(type==r.type&&!existing){auto done=r;receipt_={};report(done.source,done.id,Delivery::Registered);return;}
            if(countedVoice(type))++voices;
        }
        for(Address p=queue;p<queueEnd;p+=8){int type=at<int>(p);if(type==r.type)queued=true;if(countedVoice(type))++voices;}
        ++r.observations;auto age=now-r.began;
        if(now>r.until){auto done=r;receipt_={};report(done.source,done.id,Delivery::Unregistered);return;}
        // Receiver success means queued, not played. Recover only after the
        // native three-attempt queue has discarded this type, on the same
        // fresh play/owner, once, before any delayed cheer would be intrusive.
        if(!queued&&age>=100&&r.observations>=3){
            if(r.retried||voices>=2){auto done=r;receipt_={};report(done.source,done.id,Delivery::Unregistered);return;}
            r.retried=true;r.began=now;r.observations=0;
            if(emit(api_,receiver_,image_,entity,r.type)){++sent_;report(r.source,r.id,Delivery::Recovery);}
            else{auto done=r;receipt_={};healthy_=false;report(done.source,done.id,Delivery::Failed);}
        }
    }
    void arm(Source source,uint64_t id,int type,const sustained::Intent& wanted,uint64_t now){
        followup_={};
        if(!wanted.staggered||!wanted.playSequence||type==95)return;
        followup_={true,source,id,wanted.playSequence,now+900,now+1800,wanted.deepCheers?75:type};
    }
    bool follow(Address entity,const sustained::Intent& wanted,uint64_t now){
        if(!followup_.active)return false;
        bool same=followup_.sequence==wanted.playSequence&&wanted.staggered&&
            (followup_.source==Source::Eruption?wanted.celebration==followup_.id:
                !wanted.celebration&&wanted.reason==sustained::Reason::Result&&wanted.reactionId==followup_.sequence);
        if(!same||now>=followup_.until||now+900<followup_.due){followup_={};return false;}
        if(now<followup_.due)return false;
        if(sustained::at<Address>(entity)!=image_+bindings::ReactionVtable)return false;
        Address asset=sustained::at<Address>(entity+0x38);
        if(sustained::at<int>(asset+0x40)!=3||sustained::at<float>(asset+0x44)!=25)return false;
        Followup next=followup_;followup_={}; // One bounded attempt, consumed before native dispatch.
        Address begin=sustained::at<Address>(entity+0x120),end=sustained::at<Address>(entity+0x128);
        if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;report(Source::Followup,next.id,Delivery::Failed);return true;}
        unsigned voices=0;
        for(Address p=begin;p<end;p+=16){if(sustained::at<unsigned char>(p+12))continue;int active=sustained::at<int>(p);if(countedVoice(active))++voices;}
        // A matching family is allowed here after the deliberate gap. Keep
        // the existing two-voice budget, including the game's own instances.
        if(voices>=2){report(Source::Followup,next.id,Delivery::VoiceLimit);return true;}
        if(emit(api_,receiver_,image_,entity,next.type)){++sent_;report(Source::Followup,next.id,Delivery::Sent);}
        else{healthy_=false;report(Source::Followup,next.id,Delivery::Failed);}
        return true;
    }
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
        std::lock_guard<std::mutex> lock(mutex_);
        if(!healthy_||!fresh(wanted,wanted.match,wanted.epoch,now)){followup_={};receipt_={};return;}
        if(match_!=wanted.match||epoch_!=wanted.epoch){match_=wanted.match;epoch_=wanted.epoch;consumed_=result_=lineup_=lastLineupCheer_=liveConsumed_=lastLiveAt_=lastLiveSequence_=0;liveDelivered_=liveResolved_=0;followup_={};receipt_={};}
        checkReceipt(entity,wanted,now);if(!healthy_)return;
        bool victory=wanted.celebration&&now<wanted.celebrationUntil&&consumed_!=wanted.celebration;
        bool during=!wanted.celebration&&wanted.liveCheer&&now<wanted.liveCheerUntil&&liveConsumed_!=wanted.liveCheer;
        if(during){
            followup_={};
            if(wanted.liveType!=83&&wanted.liveType!=59&&wanted.liveType!=95&&wanted.liveType!=91&&wanted.liveType!=11)return;
            if(at<Address>(entity)!=image_+bindings::ReactionVtable)return;
            Address asset=at<Address>(entity+0x38);if(at<int>(asset+0x40)!=3||at<float>(asset+0x44)!=25)return;
            Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128);
            if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;return;}
            unsigned positives=0;bool matching=false;
            for(Address p=begin;p<end;p+=16){if(sustained::at<unsigned char>(p+12))continue;int active=at<int>(p);
                if(equivalentPositive(wanted.liveType,active))matching=true;
                if(countedVoice(active))++positives;
            }
            bool milestone=wanted.overlapMilestone&&wanted.playSequence&&lastLiveSequence_==wanted.playSequence&&now>=lastLiveAt_&&now-lastLiveAt_>=900;
            lastLiveAt_=now;lastLiveSequence_=wanted.playSequence;
            if(matching&&!milestone){liveConsumed_=wanted.liveCheer;liveDelivered_=wanted.liveCheer;report(Source::Live,wanted.liveCheer,Delivery::NativePlaying);if(wanted.majorLive)arm(Source::Live,wanted.liveCheer,wanted.liveType,wanted,now);return;}
            // Existing native reactions take the available space. Resolve this
            // opportunity now rather than adding a late cheer when one ends.
            if(positives>=2){liveConsumed_=wanted.liveCheer;report(Source::Live,wanted.liveCheer,Delivery::VoiceLimit);return;}
            liveConsumed_=wanted.liveCheer;
            if(emit(api_,receiver_,image_,entity,wanted.liveType)){++sent_;receipt(Source::Live,wanted.liveCheer,wanted.liveType,entity,wanted,now);liveDelivered_=wanted.liveCheer;lastLineupCheer_=now;report(Source::Live,wanted.liveCheer,Delivery::Sent);if(wanted.majorLive)arm(Source::Live,wanted.liveCheer,wanted.liveType,wanted,now);}
            else{healthy_=false;report(Source::Live,wanted.liveCheer,Delivery::Failed);}
            return;
        }
        bool result=!wanted.celebration&&wanted.resultCheer&&now<wanted.resultCheerUntil&&result_!=wanted.resultCheer;
        bool lineup=false;
        if(!victory&&!result&&follow(entity,wanted,now))return;
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
        followup_={};
        if(victory)consumed_=wanted.celebration;
        else if(result){result_=wanted.resultCheer;lastLineupCheer_=now;}
        else{lineup_=wanted.lineupCheer;lastLineupCheer_=now;}
        unsigned sentNow=0,nativeNow=0;bool limited=false;
        auto send=[&](int type){
            // Re-read after each send: the native instance array may reallocate.
            Address begin=at<Address>(entity+0x120),end=at<Address>(entity+0x128);
            if(end<begin||end-begin>256*16||(end-begin)%16){healthy_=false;return;}
            unsigned positives=0;
            for(Address p=begin;p<end;p+=16){if(sustained::at<unsigned char>(p+12))continue;int active=at<int>(p);if(equivalentPositive(type,active)){++nativeNow;return;}if(countedVoice(active))++positives;}
            if(positives>=2){limited=true;return;}
            if(emit(api_,receiver_,image_,entity,type)){++sent_;++sentNow;if(type!=75)receipt(victory?Source::Eruption:result?Source::Result:Source::Lineup,victory?wanted.celebration:result?wanted.resultCheer:wanted.lineupCheer,type,entity,wanted,now);}else healthy_=false;
        };
        send(result?wanted.resultType:lineup?wanted.lineupType:91);
        // Keep the immediate primary; the optional second layer has one
        // bounded delayed opportunity, never a repeated cheer barrage.
        bool stagger=wanted.staggered&&(victory||(result&&wanted.deepCheers));
        if((victory||lineup||result)&&wanted.deepCheers&&!stagger&&healthy_)send(75);
        if(stagger&&healthy_&&(sentNow||nativeNow))arm(victory?Source::Eruption:Source::Result,victory?wanted.celebration:wanted.resultCheer,result?wanted.resultType:91,wanted,now);
        report(victory?Source::Eruption:result?Source::Result:Source::Lineup,victory?wanted.celebration:result?wanted.resultCheer:wanted.lineupCheer,
            !healthy_?Delivery::Failed:sentNow?Delivery::Sent:nativeNow?Delivery::NativePlaying:limited?Delivery::VoiceLimit:Delivery::Failed);
    }
    void fail(){healthy_=false;}
    bool healthy()const{return healthy_;}unsigned sent()const{return sent_;}
    bool liveDelivered(uint64_t id)const{return id&&liveDelivered_.load()==id;}
    bool liveResolved(uint64_t id)const{return id&&liveResolved_.load()==id;}
};
}
