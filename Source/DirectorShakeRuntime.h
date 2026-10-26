#pragma once
#include "DirectorShake.h"
#include "GameBindings.h"
#include <windows.h>
#include <atomic>

namespace attendance::motion {
class PointerHook {
    uintptr_t slot_=0,original_=0,replacement_=0;
    bool exchange(uintptr_t expected,uintptr_t desired){
        DWORD protection=0;if(!VirtualProtect(reinterpret_cast<void*>(slot_),sizeof(void*),PAGE_READWRITE,&protection))return false;
        auto previous=reinterpret_cast<uintptr_t>(InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(slot_),reinterpret_cast<void*>(desired),reinterpret_cast<void*>(expected)));
        DWORD unused=0;bool restored=VirtualProtect(reinterpret_cast<void*>(slot_),sizeof(void*),protection,&unused)!=0;
        if(!restored&&previous==expected){InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(slot_),reinterpret_cast<void*>(expected),reinterpret_cast<void*>(desired));VirtualProtect(reinterpret_cast<void*>(slot_),sizeof(void*),protection,&unused);}
        return previous==expected&&restored;
    }
public:
    bool install(uintptr_t slot,uintptr_t expected,uintptr_t replacement){if(slot_||!slot||slot%sizeof(void*)||!expected||!replacement)return false;slot_=slot;original_=expected;replacement_=replacement;if(exchange(expected,replacement))return true;slot_=0;return false;}
    bool restore(){if(!slot_)return true;bool result=exchange(replacement_,original_);slot_=0;return result;}
};
class Runtime {
    uintptr_t image_=0;
    std::atomic<bool> healthy_{true};std::atomic<uint64_t> changed_{0},last_{0};std::atomic<float> percent_{0};std::atomic<Kind> kind_{Kind::None};
    static bool read(uintptr_t p,void* out,size_t size){SIZE_T n=0;return p>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,size,&n)&&n==size;}
    template<class T>static T at(uintptr_t p){T value{};read(p,&value,sizeof(value));return value;}
    bool world(const Cue& c,uintptr_t view)const{
        if(at<uintptr_t>(at<uintptr_t>(image_+bindings::MatchWrapper)+16)!=c.match||at<uintptr_t>(c.match)!=image_+0xb4781e8||at<int>(image_+0xd9e3818)!=14||at<int>(image_+0xf3f5d48)!=3)return false;
        if(at<uintptr_t>(view)!=image_+bindings::CameraVtable)return false;
        auto pause=at<uintptr_t>(image_+bindings::PauseFlag),replay=at<uintptr_t>(image_+bindings::ReplayManager);
        if(!pause||at<unsigned char>(pause)||at<uintptr_t>(replay)!=image_+bindings::ReplayVtable||at<unsigned char>(replay+0x38))return false;
        auto settings=at<uintptr_t>(image_+bindings::CameraSettings);
        if(at<uintptr_t>(settings)!=image_+bindings::CameraSettingsVtable||!at<unsigned short>(settings+0x3e6)||!at<unsigned short>(settings+0x41a))return false;
        auto owner=at<uintptr_t>(image_+bindings::CameraOwner),views=at<uintptr_t>(at<uintptr_t>(owner)+0x1f0);
        // Current render views only. No retained scene or camera references.
        for(unsigned i=0;views&&i<4;++i)if(at<uintptr_t>(views+i*0x68)==view)return true;
        return false;
    }
public:
    void configure(uintptr_t image){image_=image;}
    void tick(uintptr_t view,float* destination,const Cue& cue,uintptr_t match,unsigned epoch,uint64_t now){
        if(!healthy_||cue.match!=match||cue.epoch!=epoch||strength(cue,now)<=0||!world(cue,view))return;
        auto address=reinterpret_cast<uintptr_t>(destination);
        if(address<view+0x800&&address+sizeof(float)*16>view)return;
        std::array<float,16> projection{},output{};Cue adjusted=cue;
        Kind projectionKind=cue.kind==Kind::LivePlay||cue.kind==Kind::Safety||cue.hostileOpening>0?Kind::Touchdown:cue.kind;
        if(projectionKind==Kind::Touchdown||projectionKind==Kind::Eruption)read(view+0x2b4,projection.data(),sizeof(projection));
        float scale=projectionScale(projection,projectionKind);adjusted.percent*=scale;adjusted.impact*=scale;adjusted.transitionFrom*=scale;
        if(!read(address,output.data(),sizeof(output))||!apply(output,adjusted,now))return;
        SIZE_T n=0;if(!WriteProcessMemory(GetCurrentProcess(),destination,output.data(),sizeof(output),&n)||n!=sizeof(output))return;
        ++changed_;last_=now;percent_=strength(adjusted,now);kind_=cue.kind;
    }
    bool healthy()const{return healthy_;}void fail(){healthy_=false;}
    bool playing(uint64_t now)const{auto last=last_.load();return healthy_&&last&&now>=last&&now-last<150;}
    Kind kind()const{return kind_;}float percent()const{return percent_;}uint64_t changed()const{return changed_;}
};
}
