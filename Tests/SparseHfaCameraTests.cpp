#define WIN32_LEAN_AND_MEAN
#include "../Source/SparseHfaCamera.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace attendance::sparsecamera;
constexpr Address image=0x140000000;
struct Camera {unsigned char bytes[0x500]{};unsigned char row[16]{};unsigned refs=0;};
Camera cameras[4];unsigned gets=0,releases=0,sets=0;constexpr int off=0x1c005c;
template<class T>void put(Address p,T value){std::memcpy(reinterpret_cast<void*>(p),&value,sizeof(value));}
Address address(Camera& c){return reinterpret_cast<Address>(c.bytes);}
Camera& camera(Address p){for(auto& c:cameras)if(address(c)==p)return c;assert(false);return cameras[0];}
int current(Camera& c){return at<int>(reinterpret_cast<Address>(c.row)+8);}
void native(Camera& c,int code){put(reinterpret_cast<Address>(c.row)+8,code);}
Address __fastcall get(Address owner,Address* result,int index){assert(owner==456&&index>=1&&index<=4);auto& c=cameras[index-1];++gets;++c.refs;*result=address(c);return *result;}
int __fastcall find(Address p,const char* name){camera(p);return !std::strcmp(name,"EVENT_HFA_DISABLED")?off:off+9;}
void __fastcall set(Address p,int code,bool flag){assert(!flag);auto& c=camera(p);assert(c.refs);native(c,code);++sets;}
void __fastcall release(Address p){auto& c=camera(p);assert(c.refs);--c.refs;++releases;}
int main(){
    for(auto& c:cameras){auto p=address(c),r=reinterpret_cast<Address>(c.row);put(p,image+attendance::bindings::CameraVtable);put(p+0x420,r);put(p+0x428,r+16);put(r+4,static_cast<unsigned>(off)>>16);native(c,off+9);}
    Api api{get,find,set,release};Runtime runtime;runtime.configure(image,&api);
    Lease p{123,1,1000,true};runtime.tick(456,p,1000);
    assert(runtime.active()&&runtime.switches()==4&&gets==releases);
    for(auto& c:cameras)assert(current(c)==off&&c.refs==0);
    auto& c=cameras[0];
    int output=runtime.filter(address(c),off+7,p,1100);assert(output==off);native(c,output);
    assert(runtime.filtered()==1);
    assert(runtime.filter(address(c),0x200001,p,1100)==0x200001); // Other channels pass through unchanged.
    p.suppress=false;p.updated=1200;runtime.tick(456,p,1200);
    assert(!runtime.active()&&runtime.restores()==4&&current(c)==off+7&&gets==releases);
    for(unsigned i=1;i<4;++i)assert(current(cameras[i])==off+9);
    p.suppress=true;p.updated=1300;runtime.tick(456,p,1300);assert(current(c)==off);
    output=runtime.filter(address(c),off,p,1350);assert(output==off);native(c,output);
    p.suppress=false;p.updated=1400;runtime.tick(456,p,1400);assert(current(c)==off); // The game itself switched HFA camera off.
    native(c,off+5);p.suppress=true;p.updated=1500;runtime.tick(456,p,1500);assert(current(c)==off);
    runtime.tick(456,p,2300);assert(current(c)==off+5); // Expired suppression restores current native tier.
    assert(runtime.filter(address(c),off+9,p,2300)==off+9);
    p.updated=2400;p.epoch=2;native(c,off+3);runtime.tick(456,p,2400);assert(current(c)==off);
    p.suppress=false;p.updated=2500;runtime.tick(456,p,2500);assert(current(c)==off+3); // New scene uses its own tier.
    unsigned before=sets;p.updated=2600;runtime.tick(456,p,2600);assert(sets==before&&gets==releases);
    p.match=0;runtime.tick(456,p,2700);assert(gets==releases);
    for(auto& row:cameras)assert(row.refs==0);
    Runtime unsupported;unsupported.configure(image,&api);put(address(c),image+attendance::bindings::CameraVtable+16);
    p.match=123;p.updated=2800;unsupported.tick(456,p,2800);assert(!unsupported.healthy()&&gets==releases&&c.refs==0);
    unsigned borrowed=gets;unsupported.tick(456,p,2900);assert(gets==borrowed); // A failed optional filter never leaks references by retrying.
    put(address(c),image+attendance::bindings::CameraVtable);
    std::puts("PASS: visual-only HFA event filtering, all four current contexts, latest native tier restoration, native off/other-channel respect, pause/lease/new-scene guards and no retained references.");
}
