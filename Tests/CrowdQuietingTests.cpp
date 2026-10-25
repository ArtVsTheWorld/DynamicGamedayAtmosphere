#include "../Source/CrowdQuieting.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace attendance::quiet;
int main(){
    Intent c;c.active=true;c.match=123;c.epoch=1;c.updated=1000;
    assert(fresh(c,123,1,1000)&&fresh(c,123,1,1750));
    assert(!fresh(c,123,1,1751)&&!fresh(c,123,2,1000)&&!fresh(c,124,1,1000)&&!fresh(c,123,1,999));
    float previous=0;
    for(int i=0;i<=1000;++i){
        c.fullness=i*.001f;float level=target(c);
        assert(level>=previous&&level>=0&&level<=1);previous=level;
        for(float native:{0.0f,100.0f,350.0f,400.0f,600.0f,700.0f,1000.1f}){
            float out=apply(native,level);assert(out>=0&&out<=native);
            assert(out<=native*level+.001f);
            if(level==1)assert(out==native);
            if(level<1&&native>0)assert(out<native);
        }
        if(c.fullness>=.50f)assert(level==1);
    }
    for(float factor:{0.f,.1f,.3f,.7f,1.f}){
        float previousNative=0;
        for(int n=0;n<=1001;++n){float value=apply(static_cast<float>(n),factor);assert(value+.001f>=previousNative);previousNative=value;}
        assert(apply(1000,factor)<=1000*factor);
        assert(apply(350,factor)==350*factor); // Ordinary bed remains as tested.
    }
    assert(apply(1000,.25f)<200&&apply(1000,.25f)>180);
    for(int phase:{1,3,4,5})assert(audiblePhase(phase,true,false)&&!audiblePhase(phase,false,false));
    for(int phase:{6,7})assert(audiblePhase(phase,false,false)&&audiblePhase(phase,true,false)&&!audiblePhase(phase,true,true));
    for(int phase:{0,2,8,9})assert(!audiblePhase(phase,true,false));
    c.fullness=.15f;assert(std::fabs(apply(350,target(c))-57.5109f)<.02f);
    c.fullness=.5f;assert(target(c)==1);c.fullness=.50001f;assert(target(c)==1);
    c.fullness=.49999f;assert(target(c)<1&&target(c)>.999f);
    c.start=.35f;c.fullness=.15f;assert(std::fabs(apply(350,target(c))-98.198f)<.02f); // Preserve original listening calibration at a custom threshold.
    c.start=.50f;
    c.strength=0;assert(target(c)==1);c.strength=.5f;assert(target(c)>.5f);
    c.fullness=std::numeric_limits<float>::quiet_NaN();assert(!fresh(c,123,1,1000));
    c.fullness=.15f;c.strength=1;
    Fade fade;assert(fade.advance(target(c),0)==1);
    float value=1;
    for(int i=0;i<60;++i){float next=fade.advance(target(c),.1f);assert(next<=value);value=next;}
    assert(std::fabs(value-target(c))<.001f);
    value=fade.advance(1,.1f);assert(value>target(c)&&value<1);
    for(int i=0;i<60;++i)value=fade.advance(1,.1f);assert(value==1);
    fade.advance(0,3);fade.reset();assert(fade.advance(1,0)==1);
    c.positions=7468;c.fullness=.30f;
    for(float n:{0.f,50.f,100.f,350.f,1000.f}){
        float out=cap(apply(n,target(c)),c.positions);assert(out>=0&&out<=n&&out<100);
    }
    assert(ceiling(10000)==99&&ceiling(10001)>=99&&ceiling(10001)-99<.001f);
    float lastCeiling=0;
    for(uint64_t p=512;p<=100000;p+=17){float top=ceiling(p);assert(top>=lastCeiling&&top<=1000);lastCeiling=top;}
    c.fullness=.10f;c.positions=7468;float small=apply(350,target(c));
    c.positions=100000;float large=apply(350,target(c));assert(large>small*3&&large>100);
    assert(cap(large,c.positions)>100); // A large, sparse bowl with thousands of fans is not forced under 100.
    c.fullness=0;assert(target(c)==0);c.fullness=.5f;assert(target(c)==1);
    assert(ceiling(0)>1000&&cap(1000,0)==1000); // Unknown geometry is not guessed.
    std::puts("PASS: native quieting curve, tested calibration, full-crowd neutrality, native peak scaling, leases and six-second fades.");
}
