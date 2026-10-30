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
    std::atomic<uint64_t> appliedBegan_{0};std::atomic<float> zoomScale_{1},unscaled_{0};
    std::atomic<uint64_t> requestedBegan_{0},requestedAt_{0};std::atomic<Kind> requestedKind_{Kind::None};
    static bool read(uintptr_t p,void* out,size_t size){SIZE_T n=0;return p>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,size,&n)&&n==size;}
    template<class T>static T at(uintptr_t p){T value{};read(p,&value,sizeof(value));return value;}
    bool world(const Cue& c,uintptr_t view)const{
        if(at<uintptr_t>(at<uintptr_t>(image_+bindings::MatchWrapper)+16)!=c.match||at<uintptr_t>(c.match)!=image_+0xb47e908||at<int>(image_+0xd9eb808)!=14||at<int>(image_+0xf3fddbc)!=3)return false;
        if(at<uintptr_t>(view)!=image_+bindings::CameraVtable)return false;
        auto pause=at<uintptr_t>(image_+bindings::PauseFlag),replay=at<uintptr_t>(image_+bindings::ReplayManager);
        if(!pause||at<unsigned char>(pause)||at<uintptr_t>(replay)!=image_+bindings::ReplayVtable||at<unsigned char>(replay+0x38))return false;
        auto settings=at<uintptr_t>(image_+bindings::CameraSettings);
        if(at<uintptr_t>(settings)!=image_+bindings::CameraSettingsVtable||!at<unsigned short>(settings+0x3e6)||!at<unsigned short>(settings+0x41a))return false;
        auto owner=at<uintptr_t>(image_+bindings::CameraOwner),views=at<uintptr_t>(at<uintptr_t>(owner)+0x1f0);
        // Native CameraGet(context=1) selects this gameplay view directly.
        // Prepare the lineup envelope early, but never apply its movement to
        // the separate presentation views behind the play-selection menu.
        // Earned reactions below still follow all current live render views.
        if(c.kind==Kind::Ordinary||c.kind==Kind::Critical){
            // The snap can replace the slot-1 lineup camera. Its short fade
            // must follow the new live view instead of disappearing at the cut.
            int phase=at<int>(at<uintptr_t>(image_+bindings::GameStates)+0xa0);
            if(c.tail&&phase==3)return true;
            return views&&at<uintptr_t>(views+0x68)==view;
        }
        // Earned effects also follow native player cutaways whose borrowed
        // view is outside the four gameplay slots. The hook's verified camera
        // vtable, live match, pause/replay and cue leases remain mandatory.
        // Each output still receives its own projection compensation.
        return true;
    }
public:
    void configure(uintptr_t image){image_=image;}
    bool prepare(uintptr_t view,const Cue& cue,uintptr_t match,unsigned epoch,uint64_t now){
        if(!healthy_||cue.match!=match||cue.epoch!=epoch||strength(cue,now)<=0||!world(cue,view))return false;
        requestedBegan_=cue.began;requestedKind_=cue.kind;requestedAt_=now;return true;
    }
    void tick(uintptr_t view,float* destination,const Cue& cue,uintptr_t match,unsigned epoch,uint64_t now){
        if(!prepare(view,cue,match,epoch,now))return;
        auto address=reinterpret_cast<uintptr_t>(destination);
        if(address<view+0x800&&address+sizeof(float)*16>view)return;
        std::array<float,16> projection{},output{};Cue adjusted=cue;
        // Every added motion type softens on a zoomed view, not just scores.
        read(view+0x2b4,projection.data(),sizeof(projection));
        float scale=projectionScale(projection,cue.kind);adjusted.percent*=scale;adjusted.impact*=scale;adjusted.transitionFrom*=scale;
        if(!read(address,output.data(),sizeof(output))||!apply(output,adjusted,now))return;
        SIZE_T n=0;if(!WriteProcessMemory(GetCurrentProcess(),destination,output.data(),sizeof(output),&n)||n!=sizeof(output))return;
        ++changed_;appliedBegan_=cue.began;unscaled_=strength(cue,now);zoomScale_=scale;percent_=strength(adjusted,now);kind_=cue.kind;last_=now;
    }
    bool healthy()const{return healthy_;}void fail(){healthy_=false;}
    bool playing(uint64_t now)const{auto last=last_.load();return healthy_&&last&&now>=last&&now-last<150;}
    bool playing(const Cue& cue,uint64_t now)const{return playing(now)&&appliedBegan_==cue.began&&kind_==cue.kind;}
    bool requested(const Cue& cue,uint64_t now)const{auto at=requestedAt_.load();return at&&now>=at&&now-at<150&&requestedBegan_==cue.began&&requestedKind_==cue.kind;}
    float zoomScale()const{return zoomScale_;}float unscaled()const{return unscaled_;}
    Kind kind()const{return kind_;}float percent()const{return percent_;}uint64_t changed()const{return changed_;}
};
}
