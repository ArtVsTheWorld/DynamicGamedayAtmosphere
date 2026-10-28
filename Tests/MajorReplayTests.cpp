#include "../Source/MajorReplay.h"
#include <cassert>
#include <cstdio>
using namespace attendance::majorreplay;
int main(){
 for(auto kind:{Kind::Touchdown,Kind::Takeaway}){
  Model m;m.arm(123,1,1000,7,kind);assert(m.routing(123,1));
  assert(m.suppressInset(123,1));Input x{123,1,1100,4,true,false,false};assert(!m.tick(x));
  m.node(123,1,456,false);x.now=1200;x.replaying=true;assert(!m.tick(x));assert(m.stage()==Stage::Full);
  m.node(123,1,456,true);x.now=2200;x.replaying=false;assert(!m.tick(x));
  if(kind==Kind::Takeaway){x.now=3000;assert(!m.tick(x));m.node(123,1,789,false);x.now=3100;x.replaying=true;assert(!m.tick(x));m.node(123,1,789,true);x.now=4100;x.replaying=false;assert(!m.tick(x));}
  x.now+=249;assert(!m.tick(x));x.now++;assert(m.tick(x));assert(m.stage()==Stage::InsetRequested);
  assert(m.suppressInset(123,1));x.now+=50;assert(!m.tick(x));
  x.phase=5;x.replaying=true;x.now+=50;assert(!m.tick(x)&&m.stage()==Stage::Inset);
  x.replaying=false;x.now+=500;assert(!m.tick(x)&&m.stage()==Stage::Done);
  m.arm(123,1,x.now,7,kind);assert(m.stage()==Stage::Done); // Same result cannot replay.
  x.phase=3;x.now++;assert(!m.tick(x)&&m.stage()==Stage::Idle);
 }
 Model m;m.arm(123,1,1000,8,Kind::Touchdown);Input x{123,1,2000,4,true,true,false};m.tick(x);x.now=122000;assert(!m.tick(x));x.paused=false;x.now++;assert(!m.tick(x)&&m.stage()==Stage::WaitingFull);
 x.allowed=false;x.now++;assert(!m.tick(x)&&m.stage()==Stage::Idle);
 m.arm(123,1,1000,9,Kind::Touchdown);x={124,1,1100,4,true,false,false};assert(!m.tick(x)&&m.stage()==Stage::Idle);
 m.arm(123,1,1000,9,Kind::Touchdown);assert(!m.suppressInset(123,2));x={123,1,61001,4,true,false,false};assert(!m.tick(x)&&m.stage()==Stage::Unavailable);
 m.reset();m.arm(123,1,1000,9,Kind::Touchdown);assert(m.suppressInset(123,1));assert(m.tick(x)&&m.stage()==Stage::Unavailable);
 // Full playback can finish immediately before native play selection begins.
 m.reset();m.arm(123,1,1000,10,Kind::Touchdown);m.node(123,1,555,false);
 x={123,1,1100,4,true,false,true};m.tick(x);m.node(123,1,555,true);
 x={123,1,2000,5,true,false,false};assert(!m.tick(x));x.now=2250;assert(m.tick(x));
 // A selector refresh observes completion but cannot dispatch presentation events.
 m.reset();m.arm(123,1,1000,11,Kind::Touchdown);m.node(123,1,555,false);
 x={123,1,1100,4,true,false,true};m.tick(x);m.node(123,1,555,true);
 x={123,1,2000,4,true,false,false,false};m.tick(x);x.now=3000;assert(!m.tick(x));x.dispatch=true;assert(m.tick(x));
 std::puts("PASS: major native full-screen -> separate inset order; two-clip takeaway gaps; no cutoff during playback; once-only PIP, pause exclusion, new snap/world/disable cleanup and unavailable-recording fallback.");
}
