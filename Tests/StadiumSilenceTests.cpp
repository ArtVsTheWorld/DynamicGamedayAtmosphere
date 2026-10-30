#define WIN32_LEAN_AND_MEAN
#include "../Source/StadiumSilenceRuntime.h"
#include <cassert>
#include <cstdio>
using attendance::chants::Address;
constexpr Address image=0x140000000;
template<class T>void put(Address p,T v){memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
template<class T>T at(Address p){T v{};memcpy(&v,reinterpret_cast<void*>(p),sizeof(v));return v;}
Address alloc(){auto p=reinterpret_cast<Address>(VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;}
Address manager=0;unsigned creates=0,releases=0;bool releaseFault=false;
std::map<Address,unsigned short> handles;
Address source(){return manager;}
Address __fastcall create(Address m,Address asset){
    assert(m==manager);auto groups=at<Address>(at<Address>(asset+0x20)+0x28);assert(at<unsigned>(groups-4)==1);
    auto group=at<Address>(groups),props=at<Address>(groups+8);auto id=at<unsigned short>(group+0x28);
    assert(id==9||id==74||id==75);assert(at<unsigned short>(group+0x2a)==(id==9?2:5));
    assert(at<unsigned>(groups+0x18)==1&&at<unsigned>(props-4)==1&&at<float>(props+4)==-60);
    Address h=alloc();put(h,image+0xaf01718);put(h+8,asset);handles.emplace(h,id);++creates;return h;
}
void __fastcall release(Address m,Address h){
    assert(m==manager&&handles.count(h));
    if(releaseFault){releaseFault=false;RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);}
    ++releases;handles.erase(h);VirtualFree(reinterpret_cast<void*>(h),0,MEM_RELEASE);
}
struct Fixture {
    std::array<std::unique_ptr<attendance::chants::Preset>,3> stocks;
    Address m=alloc(),owner=alloc();
    Fixture(){
        constexpr unsigned short ids[]{9,74,75};const char* names[]{"Crowd","BandAway","BandHome"};
        for(unsigned i=0;i<3;++i){
            attendance::chants::Templates t;t.groupIndex=ids[i];t.parent=i?5:2;strcpy_s(t.name,names[i]);
            stocks[i]=std::make_unique<attendance::chants::Preset>(t,0.f,100+i);
            stocks[i]->asset.put(0x18,reinterpret_cast<Address>("Sound/CFB/CoreAudio/Utility/Mixers/VolumeSettings_MXR"));
            put(m+0x200+i*0x20,image+0xaf01718);put(m+0x208+i*0x20,stocks[i]->asset.address());put(m+0x300+i*0x20,m+0x200+i*0x20);
        }
        InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m+0x30));put(m+0xc0,m+0x300);put(m+0xc8,m+0x360);
        put(owner,image+0xb227098);put(owner+0x40,image+0xb227178);put(owner+0x48,image+0xb227188);
    }
    ~Fixture(){DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m+0x30));VirtualFree(reinterpret_cast<void*>(m),0,MEM_RELEASE);VirtualFree(reinterpret_cast<void*>(owner),0,MEM_RELEASE);}
};
int main(){
    Fixture a,b;manager=a.m;attendance::silence::Runtime mute;mute.configure(image,nullptr,create,release,source);
    mute.tick(a.owner+0x40,false);assert(!creates&&!mute.active());
    mute.tick(a.owner+0x40,true);assert(creates==3&&handles.size()==3&&mute.active());
    for(unsigned i=0;i<3;++i)assert(mute.muted(i));
    for(unsigned i=0;i<100;++i)mute.tick(a.owner+0x40,true);assert(creates==3);
    mute.tick(a.owner+0x40,false);assert(creates==releases&&handles.empty()&&!mute.active());
    mute.tick(a.owner+0x40,true);assert(creates==6);
    // Manager teardown retires its handles; never release them via the new manager.
    for(auto [h,id]:handles){(void)id;VirtualFree(reinterpret_cast<void*>(h),0,MEM_RELEASE);}handles.clear();manager=b.m;
    mute.tick(b.owner+0x40,false);assert(releases==3&&!mute.active());
    mute.tick(b.owner+0x40,true);assert(creates==9&&mute.healthy());
    releaseFault=true;mute.tick(b.owner+0x40,false);assert(!mute.healthy()&&mute.active()&&handles.size()==1);
    mute.tick(b.owner+0x40,false);assert(!mute.active()&&handles.empty());
    unsigned before=creates;mute.tick(b.owner+0x40,true);assert(creates==before+2); // Faulted bus cannot restart; other buses remain independent.
    mute.tick(b.owner+0x40,false);assert(handles.empty());
    // A different music group with an accidentally reused ID cannot be muted.
    strcpy_s(b.stocks[2]->groupName,"PAMusic");
    attendance::silence::Runtime strict;strict.configure(image,nullptr,create,release,source);before=creates;
    strict.tick(b.owner+0x40,true);assert(creates==before+2&&!strict.muted(2));strict.tick(b.owner+0x40,false);
    std::puts("PASS: crowd and both bands independently mute, remain stable, restore, validate identity and retry fault retirement without touching other groups.");
}
