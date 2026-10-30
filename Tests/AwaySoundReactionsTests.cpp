#include "../Source/AwaySoundReactions.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
struct Fixture {
 awaysound::Model model;energy::Settings energy;live::Settings live;energy::Play p;skills::Sample sample;uint64_t time=1000,sequence=1;bool allowed=true;
 Fixture(int offense=1,bool neutral=false){p.game={2,7,7,1,400,720};p.game.neutral=neutral;p.offense=offense;p.down=1;p.selectedType=103;p.toGoal=60;p.toFirst=10;p.fieldSpot=-10;p.attackDirection=1;}
 awaysound::Outcome tick(int phase,int kind=0,int ball=2,float spot=-10,bool held=true){p.game.phase=phase;time+=100;if(phase==3&&kind==3&&ball==1){p.passReleased=true;p.resultStamp=1;}model.track(energy,p,sequence);return model.observe(live,p,123,1,sequence,time,allowed,456,kind,ball,spot,spot,held,false,sample);}
 void begin(){tick(1);tick(3);}
};
int main(){
 for(bool neutral:{false,true}){Fixture f(1,neutral);f.begin();f.tick(3,3,1,10,false);auto catchSound=f.tick(3,1,4,10);assert(bool(catchSound.type)==!neutral);if(!neutral)assert(catchSound.type/4==5);assert(!f.tick(3,1,4,20).type);
  f.p.game.awayScore+=6;f.p.game.seconds-=8;auto score=f.tick(4,1,4,50);assert(bool(score.type)==!neutral);if(!neutral)assert(score.scoring&&score.type/4==17);assert(!f.tick(5).type);
 }
 // The existing visitor-deep-catch route can speak before the20-yard shadow
 // threshold. Cover it across ticks without blocking a later verified TD.
 {Fixture f;f.begin();f.tick(3,3,1,5,false);f.model.cover(f.sequence);
  assert(!f.tick(3,1,4,10).type);assert(!f.tick(3,1,4,20).type);
  f.p.game.awayScore+=6;f.p.game.seconds-=8;auto score=f.tick(4,1,4,50);assert(score.scoring&&score.type/4==17);assert(!f.tick(5).type);
  ++f.sequence;f.p.resultStamp+=1;f.begin();f.tick(3,3,1,10,false);assert(f.tick(3,1,4,10).type);}
 {Fixture f;f.begin();f.model.cover(f.sequence,true);f.p.game.awayScore+=6;f.p.game.seconds-=8;assert(!f.tick(4).type);}
 // A home success cannot produce its opponent's disappointment counterpart.
 {Fixture f(0);f.begin();f.tick(3,3,1,10,false);assert(!f.tick(3,1,4,10).type);f.p.game.homeScore+=6;f.p.game.seconds-=8;assert(!f.tick(4,1,4,50).type);}
 // A home third-down incompletion becomes the existing away defensive-stop sound.
 {Fixture f(0);f.p.down=3;f.begin();f.tick(3,3,1,0,false);f.p.down=4;f.p.game.seconds-=5;auto out=f.tick(4,3,1,0,false);assert(out.type/4==11);assert(!f.tick(5).type);}
 for(int unavailable:{0,1,2,3}){Fixture f;f.allowed=unavailable!=0;f.p.simulated=unavailable==1;f.live.enabled=unavailable!=2;f.p.selectedType=unavailable==3?37:103;f.begin();f.tick(3,3,1,10,false);assert(!f.tick(3,1,4,10).type);f.allowed=true;f.p.simulated=false;f.live.enabled=true;assert(!f.tick(4,1,4,10).type);}
 // No fresh ready/live proof after a discontinuity; no score from a stale graph.
 {Fixture f;f.begin();f.model.reset();f.tick(3,3,1,10,false);assert(!f.tick(3,1,4,10).type);f.p.game.awayScore+=6;assert(!f.tick(4).type);}
 // Main and shadow serials may differ after a gap; each proof stays on current main play.
 {Fixture f;f.sequence=31;f.begin();f.p.game.awayScore+=6;f.p.game.seconds-=5;auto out=f.tick(4);assert(out.scoring&&out.type/4==19);assert(!f.tick(5).type);}
 std::puts("PASS: away-only existing-trigger counterparts, live-to-score upgrade once, split-crowd/home ownership, disabled/sim/kneel/gap exclusion and main-play sequence alignment.");
}
