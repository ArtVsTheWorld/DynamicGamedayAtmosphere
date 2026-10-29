#include "../Source/AttendanceAudio.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace attendance;
int main(){
 const float low[]={-11.75f,-9.5f,-9.5f,-7.75f,-17.5f,.75f,-19.f};
 const float mid[]={-5.75f,-8.25f,-8.25f,-4.75f,-8.25f,-2.25f,-8.75f};
 auto small=balance::mix(.3f,6060);auto half=balance::mix(.5f,6060);
 for(unsigned i=0;i<7;++i){
  // Independent signal-domain check against the previous listening anchors;
  // quarter-dB quantization allows at most .125 dB error.
  float expectedLow=low[i]<0?20*std::log10((1+std::pow(10.f,low[i]/20))/2):low[i];
  float expectedMid=mid[i]<0?20*std::log10((1+std::pow(10.f,mid[i]/20))/2):mid[i];
  assert(std::fabs(small.net(i)-expectedLow)<=.1251f);assert(std::fabs(half.net(i)-expectedMid)<=.1251f);
 }
 assert(small.net(1)==-3.5f&&half.net(1)>small.net(1));
 assert(balance::mix(.7f,6060).needed()==false);
 assert(balance::mix(.9f,6060).net(1)==1);
 assert(balance::mix(.5f,61888).net(1)>0&&half.net(1)<0);
 assert(!balance::mix(.5f,0).valid&&!balance::mix(-1,6060).valid&&!balance::mix(std::numeric_limits<float>::quiet_NaN(),6060).valid);
 for(uint64_t seats:{6060,12000,20000,30000,61888}){
  auto zero=balance::mix(0,seats);assert(zero.mute&&zero.needed()&&zero.gain[0]==-60);
  auto crossing=balance::mix(balance::crossover(seats),seats);assert(!crossing.needed());
  float lastBed=-1000,lastReaction=-1000;
  for(int n=1;n<=1000;++n){auto m=balance::mix(n*.001f,seats);assert(m.valid&&!m.mute);
   for(unsigned i=0;i<7;++i){assert(m.net(i)<=volume::boostDb(30)+.0001f&&m.gain[i+1]<=.001f);}
   assert(m.net(1)==m.net(2));assert(m.net(1)>=lastBed-.001f&&m.net(3)>=lastReaction-.001f);lastBed=m.net(1);lastReaction=m.net(3);
  }
  auto off=balance::mix(1,seats,0);assert(!off.needed());
  auto capped=balance::mix(1,seats,17.5f);assert(capped.net(1)<=volume::boostDb(17.5f)+.0001f);
 }
 assert(std::fabs(balance::mix(1,61888).net(1)-volume::boostDb(30))<.0001f);
 std::puts("PASS: half-cut signal anchors, occupied-size crossover, chant parity, native dynamics gain bounds, zero mute and invalid-context fallback.");
}
