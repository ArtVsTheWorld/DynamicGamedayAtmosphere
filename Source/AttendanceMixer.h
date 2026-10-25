#pragma once
#include "ChantMixerRuntime.h"
#include "SwellAudioRuntime.h"
#include "AttendanceAudio.h"

namespace attendance::balance {
using namespace chants;
struct Preset {
    chants::Preset crowd,chant;
    Array<0x50> groups;
    Preset(const Templates& a,const Templates& b,float parent,float offset,uint64_t serial):crowd(a,parent,serial*2),chant(b,offset,serial*2+1){
        groups.count=2;
        std::memcpy(groups.data.data(),crowd.groups.data.data(),0x28);
        std::memcpy(groups.data.data()+0x28,chant.groups.data.data(),0x28);
        crowd.preset.put(0x28,groups.address());
    }
};
// One native preset owns both related gains. Their fades start together,
// including the chant compensation when the parent rises for an eruption.
class Runtime {
    Address image_=0,manager_=0,handle_=0,asset_=0;
    Create create_=nullptr;Release release_=nullptr;Manager source_=nullptr;Clock clock_=nullptr;
    chants::Runtime reader_;Templates crowd_,chant_;bool templates_=false;
    std::map<std::pair<int,int>,std::unique_ptr<Preset>> presets_;
    std::mutex mutex_;std::atomic<bool> healthy_{true};
    std::atomic<float> parent_{0},chantNet_{0};uint64_t changed_=0;
    bool create(Address asset,Address& out){__try{out=create_(manager_,asset);return out!=0;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
    bool release(){__try{release_(manager_,handle_);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
public:
    void configure(Address image,Create make=nullptr,Release end=nullptr,Manager source=nullptr,Clock clock=nullptr){
        image_=image;create_=make?make:reinterpret_cast<Create>(image+bindings::MixerCreate);release_=end?end:reinterpret_cast<Release>(image+bindings::MixerRelease);source_=source;clock_=clock;reader_.configure(image);
    }
    void tick(Address owner,bool active,float percent,float chantPercent){
        if(!healthy_||!std::isfinite(percent)||!std::isfinite(chantPercent)||owner<0x10040||
           sustained::at<Address>(owner-0x40)!=image_+0xb220ce8||sustained::at<Address>(owner)!=image_+0xb220dc8||sustained::at<Address>(owner+8)!=image_+0xb220dd8)return;
        std::lock_guard<std::mutex> guard(mutex_);
        Address manager=source_?source_():sustained::at<Address>(image_+bindings::MixerManager);
        if(manager!=manager_){manager_=manager;handle_=asset_=0;parent_=chantNet_=0;templates_=false;changed_=0;}
        if(!manager_)return;
        Mix wanted=active?mixer(percent,chantPercent):Mix{};
        bool needed=wanted.crowd!=0||wanted.chant!=0;
        if(handle_&&wanted.crowd==parent_&&wanted.crowd+wanted.chant==chantNet_)return;
        auto now=clock_?clock_():GetTickCount64();
        // Keep native quarter-second transitions from accumulating. Cleanup
        // and downward cap changes still happen on the next owning callback.
        if(handle_&&needed&&wanted.crowd>=parent_&&now>=changed_&&now-changed_<300)return;
        if(handle_){
            if(sustained::at<Address>(handle_)!=image_+0xaefb4b8||(sustained::at<Address>(handle_+8)&~Address(4))!=asset_||!release()){healthy_=false;return;}
            handle_=asset_=0;parent_=chantNet_=0;
        }
        if(!needed)return;
        if(!templates_){
            if(!reader_.snapshot(manager_,9,crowd_)||!reader_.snapshot(manager_,14,chant_))return;
            if(crowd_.parent!=2||std::strcmp(crowd_.name,"Crowd")||chant_.parent!=9||std::strcmp(chant_.name,"Chants/Response"))return;
            templates_=true;
        }
        auto key=std::make_pair(wanted.crowdKey,wanted.chantKey);
        auto found=presets_.find(key);
        if(found==presets_.end())found=presets_.emplace(key,std::make_unique<Preset>(crowd_,chant_,wanted.crowd,wanted.chant,static_cast<uint64_t>(presets_.size()+1))).first;
        Address asset=found->second->crowd.asset.address(),handle=0;
        if(!create(asset,handle)||sustained::at<Address>(handle)!=image_+0xaefb4b8||(sustained::at<Address>(handle+8)&~Address(4))!=asset){healthy_=false;return;}
        handle_=handle;asset_=asset;parent_=wanted.crowd;chantNet_=wanted.crowd+wanted.chant;changed_=now;
    }
    float db()const{return parent_;}float chantDb()const{return chantNet_;}bool healthy()const{return healthy_;}
};
}
