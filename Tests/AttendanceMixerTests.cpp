#define WIN32_LEAN_AND_MEAN
#include "../Source/AttendanceMixer.h"
#include <cassert>
#include <cstdio>
using attendance::chants::Address;
constexpr Address image=0x140000000;
template<class T>void put(Address p,T v){memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));}
template<class T>T at(Address p){return attendance::sustained::at<T>(p);}
Address alloc(){Address p=reinterpret_cast<Address>(VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(p);return p;}
Address manager=0,handle=0;unsigned creates=0,releases=0;uint64_t now=1000;
Address source(){return manager;}uint64_t clockValue(){return now;}
Address __fastcall create(Address m,Address asset){
    assert(m==manager);++creates;Address preset=at<Address>(asset+0x20),groups=at<Address>(preset+0x28);assert(at<unsigned>(groups-4)==2);
    for(unsigned i=0;i<2;++i){Address entry=groups+i*0x28,group=at<Address>(entry),props=at<Address>(entry+8);assert(at<unsigned short>(group+0x28)==(i?14:9)&&at<unsigned short>(group+0x2a)==(i?9:2));assert(at<unsigned>(props-4)==1&&at<unsigned>(props)==0);assert(at<unsigned char>(entry+0x20)==1&&at<unsigned char>(entry+0x21)==0);}
    handle=alloc();put(handle,image+0xaefb4b8);put(handle+8,asset);return handle;
}
void __fastcall release(Address m,Address h){assert(m==manager&&h==handle);++releases;assert(VirtualFree(reinterpret_cast<void*>(h),0,MEM_RELEASE));handle=0;}
struct Fixture {
    attendance::chants::Templates a,b;std::unique_ptr<attendance::balance::Preset> stock;Address m=alloc(),owner=alloc();
    Fixture(){a.groupIndex=9;a.parent=2;strcpy_s(a.name,"Crowd");stock=std::make_unique<attendance::balance::Preset>(a,b,0.f,0.f,123ull);stock->crowd.asset.put(0x18,reinterpret_cast<Address>("Sound/CFB/CoreAudio/Utility/Mixers/VolumeSettings_MXR"));
        InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m+0x30));put(m+0x200,image+0xaefb4b8);put(m+0x208,stock->crowd.asset.address());put(m+0x300,m+0x200);put(m+0xc0,m+0x300);put(m+0xc8,m+0x320);
        put(owner,image+0xb220ce8);put(owner+0x40,image+0xb220dc8);put(owner+0x48,image+0xb220dd8);
    }
    ~Fixture(){DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m+0x30));VirtualFree(reinterpret_cast<void*>(m),0,MEM_RELEASE);VirtualFree(reinterpret_cast<void*>(owner),0,MEM_RELEASE);}
};
int main(){
    Fixture a,b;manager=a.m;attendance::balance::Runtime mix;mix.configure(image,create,release,source,clockValue);
    mix.tick(a.owner+0x40,true,0,0);assert(!creates);
    mix.tick(a.owner+0x40,true,25,10);assert(mix.healthy()&&std::fabs(mix.db()-attendance::volume::boostDb(25))<.001f&&std::fabs(mix.chantDb()-attendance::volume::boostDb(10))<.001f&&creates==1);
    for(unsigned i=0;i<100;++i)mix.tick(a.owner+0x40,true,25,10);assert(creates==1);
    now+=400;mix.tick(a.owner+0x40,true,50,10);assert(std::fabs(mix.db()-attendance::volume::boostDb(50))<.001f&&std::fabs(mix.chantDb()-attendance::volume::boostDb(10))<.001f);
    now+=400;mix.tick(a.owner+0x40,true,20,0);assert(std::fabs(mix.db()-attendance::volume::boostDb(20))<.001f&&mix.chantDb()==0);
    mix.tick(a.owner+0x40,false,70,10);assert(mix.db()==0&&mix.chantDb()==0&&creates==releases);
    now+=400;mix.tick(a.owner+0x40,true,25,10);assert(handle);assert(VirtualFree(reinterpret_cast<void*>(handle),0,MEM_RELEASE));handle=0;manager=b.m;unsigned old=releases;
    mix.tick(b.owner+0x40,false,0,0);assert(releases==old&&mix.db()==0); // Replaced manager owns retirement of its former handles.
    now+=400;mix.tick(b.owner+0x40,true,25,10);assert(mix.healthy()&&std::fabs(mix.chantDb()-attendance::volume::boostDb(10))<.001f);mix.tick(b.owner+0x40,false,0,0);assert(!handle);
    now+=400;mix.tick(b.owner+0x40,true,50,15);assert(mix.healthy()&&std::fabs(mix.db()-attendance::volume::boostDb(50))<.001f&&std::fabs(mix.chantDb()-attendance::volume::boostDb(15))<.001f);
    now+=400;mix.tick(b.owner+0x40,true,0,15);assert(mix.db()==0&&std::fabs(mix.chantDb()-attendance::volume::boostDb(15))<.001f); // Independent chant-only boost also stays in its own group.
    mix.tick(b.owner+0x40,false,0,0);assert(!handle&&creates==releases+1); // One old manager-owned handle was retired earlier.
    std::puts("PASS: one atomic two-group preset, independent 15-percent chant ceiling during crowd/eruption boosts, native ownership, stable updates, manager replacement and cleanup.");
}
