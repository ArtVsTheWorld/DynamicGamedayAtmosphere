#define WIN32_LEAN_AND_MEAN
#include "../Source/CrowdIntensityRuntime.h"
#include <cassert>
#include <cstdio>
using namespace attendance::sustained;
constexpr Address image=0x140000000;
unsigned locks=0,unlocks=0,publishes=0;float last=0;bool fault=false,reentrant=false;
IntensityRuntime* recursive=nullptr;Intent intent;Address current=0;uint64_t timeValue=1000;
template<class T>void put(Address p,T value){memcpy(reinterpret_cast<void*>(p),&value,sizeof(value));}
void __fastcall lock(Address p){assert(p==current+0x90);++locks;}
void __fastcall unlock(Address p){assert(p==current+0x90);++unlocks;}
void __fastcall publish(Address p,const float* value,bool force){
    assert(p==current+0x80&&force&&locks==unlocks+1);last=*value;++publishes;
    if(reentrant)recursive->tick(current,intent,123,1,timeValue);
    if(fault){fault=false;RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);}
}
int main(){
    Address entity=reinterpret_cast<Address>(VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(entity);
    unsigned char asset[0x100]{};Address a=reinterpret_cast<Address>(asset);
    put(entity,image+attendance::bindings::IntensityVtable);put(entity+0x38,a);put(entity+0x80,Address(0x12345));put(entity+0xc4,200.0f);
    put(a+0x3c,0.0f);put(a+0x40,1000.0f);
    IntensityRuntime runtime;runtime.configure(image,lock,unlock,publish);recursive=&runtime;current=entity;
    intent.match=123;intent.epoch=1;intent.active=true;intent.intensity=1000;
    auto tick=[&](uint64_t now){timeValue=now;intent.updated=now;runtime.tick(current,intent,123,1,now);assert(locks==unlocks);};
    reentrant=true;for(uint64_t now=1000;now<=3100;now+=100)tick(now);reentrant=false;
    assert(std::fabs(last-1000)<.01f&&runtime.native()==200);
    assert(at<float>(entity+0xc4)==200); // Native envelope and timers were never changed.
    put(entity+0xc4,990.0f);intent.intensity=800;tick(3200);assert(last==990); // Never suppress a native eruption.
    intent.intensity=1000;tick(3300);put(entity+0xc4,340.0f);
    runtime.tick(current,intent,123,1,4100);assert(last==340&&runtime.restores()>=1&&locks==unlocks); // Expired lease restores contemporaneous native intensity.
    intent.active=true;for(uint64_t now=4200;now<=6400;now+=100)tick(now);assert(last==1000);
    intent.active=false;tick(6500);assert(last==340); // Pause, replay, disabled audio and unsupported context share this gate.
    put(entity+0xc4,1000.107f);tick(6550);assert(runtime.native()>1000&&runtime.output()==runtime.native()); // A fractional native overshoot must still permit restoration.
    put(entity+0xc4,340.0f);
    intent.active=true;for(uint64_t now=6600;now<=8800;now+=100)tick(now);
    put(entity+0x80,Address(0x23456));put(entity+0xc4,700.0f);unsigned before=publishes;tick(8900);assert(publishes==before); // Fresh binding starts from native.
    tick(9000);assert(last>=700&&last<=1000);
    fault=true;tick(9100);assert(!runtime.healthy()&&locks==unlocks);
    put(entity+0xc4,410.0f);tick(9200);assert(last==410&&locks==unlocks); // A publish fault still restores safely on its owning callback.
    assert(VirtualFree(reinterpret_cast<void*>(entity),0,MEM_RELEASE));
    current=reinterpret_cast<Address>(VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(current);
    put(current,image+attendance::bindings::IntensityVtable);put(current+0x38,a);put(current+0x80,Address(0x34567));put(current+0xc4,250.0f);
    tick(9400);assert(runtime.native()==250&&runtime.output()==250); // No access to the retired object.
    IntensityRuntime reaction;reaction.configure(image,lock,unlock,publish);intent.active=true;intent.reason=Reason::Result;intent.intensity=1000;
    intent.updated=9500;reaction.tick(current,intent,123,1,9500);
    intent.updated=9750;reaction.tick(current,intent,123,1,9750);assert(last==1000&&at<float>(current+0xc4)==250);
    put(current+0xc4,1000.1f);intent.intensity=800;intent.updated=9800;reaction.tick(current,intent,123,1,9800);assert(last==1000.1f); // A natural cheer always keeps its stronger intensity.
    intent.active=false;intent.updated=9900;reaction.tick(current,intent,123,1,9900);assert(last==1000.1f&&locks==unlocks);
    IntensityRuntime lineup;lineup.configure(image,lock,unlock,publish);put(current+0xc4,400.0f);intent.active=true;intent.reason=Reason::Lineup;intent.intensity=950;
    intent.updated=10000;lineup.tick(current,intent,123,1,10000);intent.updated=10250;lineup.tick(current,intent,123,1,10250);
    assert(last==950&&at<float>(current+0xc4)==400); // The early cheer has excited voices before the slower roar attack finishes.
    intent.active=false;intent.updated=10300;lineup.tick(current,intent,123,1,10300);assert(last==400&&locks==unlocks);
    // Verified Beds selector: lower excitement without changing envelope, gains or HFA.
    unsigned char publisher[0x80]{},selector[0xc0]{};
    Address pub=reinterpret_cast<Address>(publisher),sel=reinterpret_cast<Address>(selector);
    put(current+0x80,pub);put(pub+0x18,sel);put(sel,image+0xb0b49b0);put(sel+0x40,0);put(current+0xc4,350.0f);
    IntensityRuntime calmer;calmer.configure(image,lock,unlock,publish);intent.active=false;
    attendance::quiet::Intent low;low.match=123;low.epoch=1;low.active=true;low.fullness=.15f;low.start=.35f;
    auto quietTick=[&](uint64_t now){low.updated=now;intent.updated=now;calmer.tick(current,intent,123,1,now,low);assert(locks==unlocks);};
    for(uint64_t now=11000;now<=17100;now+=100)quietTick(now);
    assert(std::fabs(last-98.198f)<.02f&&at<float>(current+0xc4)==350);
    put(current+0xc4,1000.1f);quietTick(17200);assert(std::fabs(last-attendance::quiet::apply(1000.1f,attendance::quiet::target(low)))<.01f); // Native peaks follow the same sparse-crowd correction.
    put(current+0xc4,700.0f);quietTick(17300);assert(std::fabs(calmer.output()-attendance::quiet::apply(700,attendance::quiet::target(low)))<.01f);
    put(current+0xc4,350.0f);quietTick(17400);assert(last<100);
    intent.active=true;intent.intensity=1000;intent.reason=Reason::Result;
    quietTick(17500);quietTick(17750);assert(last==1000); // Added reaction takes priority over quiet bed.
    for(float tail:{500.0f,400.0f,350.0f,300.0f,200.0f,100.0f}){
        intent.intensity=tail;quietTick(17760);assert(std::fabs(last-tail)<.01f);
    } // Reaction tail crosses the native 350 bed without dropping abruptly to 100.
    intent.active=false;quietTick(17800);assert(last<100);
    low.active=false;quietTick(17900);assert(last==350); // Pause/replay/disabled context restores immediately.
    low.active=true;for(uint64_t now=18000;now<=24100;now+=100)quietTick(now);assert(last<100);
    put(sel+0x40,1);quietTick(24200);assert(last==350); // Other audio routes are never quieted.
    put(sel+0x40,0);for(uint64_t now=24300;now<=30400;now+=100)quietTick(now);assert(last<100);
    low.updated=30400;calmer.tick(current,intent,123,1,31200,low);assert(last==350); // Expired quiet lease.
    for(uint64_t now=31300;now<=37400;now+=100)quietTick(now);assert(last<100);
    low.updated=37500;calmer.tick(current,intent,123,2,37500,low);assert(last==350); // Same object, new match epoch restores.
    for(uint64_t now=37600;now<=43700;now+=100)quietTick(now);assert(last<100);
    low.strength=0;for(uint64_t now=43800;now<=49900;now+=100)quietTick(now);assert(std::fabs(last-350)<.01f&&calmer.output()==350); // Live disable fades to native.
    low.strength=1;low.start=.5f;low.fullness=.5f;
    for(uint64_t now=50000;now<=56100;now+=100)quietTick(now);assert(calmer.output()==350);
    low.fullness=.499f;for(uint64_t now=56200;now<=62300;now+=100)quietTick(now);assert(calmer.output()<350);
    low.fullness=.5001f;for(uint64_t now=62400;now<=68500;now+=100)quietTick(now);assert(calmer.output()==350);
    IntensityRuntime intro;intro.configure(image,lock,unlock,publish);low.fullness=.15f;low.presentation=true;low.updated=69000;
    intro.tick(current,intent,123,1,69000,low);assert(std::fabs(intro.output()-attendance::quiet::apply(350,attendance::quiet::target(low)))<.01f); // No full-volume burst on a fresh intro entity.
    low.active=false;intro.tick(current,intent,123,1,69100,low);assert(intro.output()==350);
    IntensityRuntime smallVenue;smallVenue.configure(image,lock,unlock,publish);
    low.active=true;low.fullness=.30f;low.positions=7468;low.presentation=true;low.updated=70000;
    put(current+0xc4,1000.f);smallVenue.tick(current,intent,123,1,70000,low);
    assert(smallVenue.output()>400&&smallVenue.output()<500&&at<float>(current+0xc4)==1000); // Native peak retains crowd-scaled dynamics even on the first intro callback.
    low.presentation=false;
    for(uint64_t now=70100;now<=77000;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);assert(smallVenue.output()>400&&smallVenue.output()<500);}
    low.positions=10001;low.updated=77100;smallVenue.tick(current,intent,123,1,77100,low);assert(smallVenue.output()>400&&smallVenue.output()<500);
    low.fullness=.5f;
    for(uint64_t now=77200;now<=83400;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);}
    assert(smallVenue.output()==1000); // At the threshold, both filters restore smoothly.
    low.fullness=.1f;low.positions=100000;
    for(uint64_t now=83500;now<=89900;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);}
    assert(smallVenue.output()>100&&smallVenue.output()<1000);
    low.positions=7468;low.strength=0;
    for(uint64_t now=90000;now<=96400;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);}
    assert(smallVenue.output()==1000);
    // Actual Wake status: 48.4%, 18,829 mapped positions, native 1000 peak.
    // Test the real borrowed-entity runtime, with no added reaction or gain.
    low.strength=1;low.fullness=.484f;low.positions=18829;
    for(uint64_t now=96500;now<=103000;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);}
    assert(smallVenue.native()==1000&&smallVenue.output()>940&&smallVenue.output()<1000);
    put(current+0xc4,350.f);low.updated=103100;smallVenue.tick(current,intent,123,1,103100,low);
    assert(smallVenue.output()>320&&smallVenue.output()<350); // Background remains calmer.
    low.fullness=.5001f;
    for(uint64_t now=103200;now<=109600;now+=100){low.updated=now;smallVenue.tick(current,intent,123,1,now,low);}
    assert(smallVenue.output()==350&&at<float>(current+0xc4)==350&&locks==unlocks);
    assert(VirtualFree(reinterpret_cast<void*>(current),0,MEM_RELEASE));
    std::puts("PASS: output-only excitement, gradual attack, native floors, reentrancy, lease/disable restoration, publisher replacement, retired objects and native-call faults.");
}
