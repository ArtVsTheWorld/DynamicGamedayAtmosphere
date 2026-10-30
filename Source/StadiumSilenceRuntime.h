#pragma once
#include "ChantMixerRuntime.h"
namespace attendance::silence {
// Verified native groups: Crowd9(parent2), BandAway74/BandHome75(parent5).
// Owned mute properties affect those roots only; other music/field/UI groups
// and the user's sliders are untouched. Retire through the owning callback.
class Runtime {
    std::array<chants::Runtime,3> buses_;
    inline static constexpr std::array<unsigned short,3> groups_{9,74,75};
public:
    void configure(uintptr_t image,chants::Note note=nullptr,chants::Create create=nullptr,chants::Release release=nullptr,chants::Manager manager=nullptr){
        for(auto& bus:buses_)bus.configure(image,note,create,release,manager);
    }
    bool active()const{for(const auto& bus:buses_)if(bus.db()!=0)return true;return false;}
    bool healthy()const{for(const auto& bus:buses_)if(!bus.healthy())return false;return true;}
    void tick(uintptr_t owner,bool wanted){
        if(!wanted&&!active())return;
        for(unsigned i=0;i<buses_.size();++i)buses_[i].tick(owner,wanted,-60,false,true,groups_[i]);
    }
    bool muted(unsigned i)const{return i<buses_.size()&&buses_[i].db()==-60;}
};
}
