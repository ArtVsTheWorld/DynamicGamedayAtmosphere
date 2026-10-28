#include "../Source/StartingAttendance.h"
#include "../Source/ArrivalRules.h"
#include <cassert>
#include <cstdio>
using namespace attendance::turnout;
Seed sample(){Seed s;s.home={1153,50,70,6,3,3,3};s.away={1193,60,80,5,3,3,3};s.stadium=s.homeStadium=553;s.capacity=60000;s.schools=139;s.week=8;s.year=2026;s.ea=45000;s.regular=true;return s;}
Launch launch(const Seed& s){return {s.home.id,s.away.id,s.stadium,s.capacity,s.ea,720,0,false,false};}
bool near(float a,float b){return std::fabs(a-b)<.001f;}
int main(){
    Seed s=sample();Launch l=launch(s);auto base=calculate(s,l);assert(base.applied&&base.attendance>0&&base.attendance<=s.capacity);
    l.minutes=1139;assert(!calculate(s,l).night);l.minutes=1140;auto night=calculate(s,l);assert(night.night&&near(night.percent-base.percent,7)&&night.kickoff==97);l.minutes=720;
    l.precipitation=1;assert(near(base.percent-calculate(s,l).percent,5));l.precipitation=2;assert(near(base.percent-calculate(s,l).percent,9));l.precipitation=3;assert(near(base.percent-calculate(s,l).percent,14));l.precipitation=0;
    Seed indoor=s;indoor.roof=1;l.precipitation=3;assert(calculate(indoor,l).weather==0);l.precipitation=0;
    l.rivalry=true;assert(near(calculate(s,l).percent-base.percent,10));l.rivalry=false;
    Seed opener=s;opener.home.homeGames=0;assert(near(calculate(opener,l).percent-base.percent,4));
    Seed winning=s;winning.home.wins=6;winning.home.losses=0;assert(near(calculate(winning,l).momentum,8));
    Seed losing=s;losing.home.wins=0;losing.home.losses=6;assert(near(calculate(losing,l).momentum,-10));
    Seed early=losing;early.home.losses=1;assert(calculate(early,l).momentum>-2); // One loss cannot manufacture a season-long collapse.
    Seed fcs=s;fcs.away.id=1500;fcs.away.rank=256;l=launch(fcs);assert(calculate(fcs,l).appeal==-8);
    l=launch(s);Seed unknown=s;unknown.home.toughest=0;assert(calculate(unknown,l).applied&&near(calculate(unknown,l).demand,75));
    l.neutral=true;l.rivalry=true;auto neutral=calculate(s,l);std::swap(s.home,s.away);l.home=s.home.id;l.away=s.away.id;auto reverse=calculate(s,l);assert(near(neutral.percent,reverse.percent)&&neutral.attendance==reverse.attendance);
    s=sample();l=launch(s);
    for(int reason=0;reason<9;++reason){Seed bad=s;Launch wrong=l;
        if(reason==0)bad.regular=false;if(reason==1)bad.resumed=true;if(reason==2)bad.schools=0;if(reason==3)wrong.minutes=1440;if(reason==4)wrong.precipitation=5;
        if(reason==5)wrong.home++;if(reason==6)wrong.stadium++;if(reason==7)wrong.ea++;if(reason==8)wrong.capacity=0;
        auto r=calculate(bad,wrong);assert(!r.applied&&r.attendance==wrong.ea);
    }
    for(int rank:{1,25,70,139})for(int record:{0,3,6})for(int rain:{0,1,2,3})for(int time:{720,1140}){
        s=sample();s.home.toughest=rank;s.home.wins=record;s.home.losses=6-record;l=launch(s);l.precipitation=rain;l.minutes=time;
        auto a=calculate(s,l),b=calculate(s,l);assert(a.applied&&a.percent>=30&&a.percent<=100&&a.attendance<=l.capacity&&a.attendance==b.attendance&&a.variation>=-3&&a.variation<=3);
    }
    s=sample();l=launch(s);Seeds cache;Seed found;cache.remember(s,1000);Launch unrelated=l;unrelated.away++;assert(!cache.find(unrelated,1100,found));assert(cache.find(l,1100,found));auto serial=found.serial;assert(cache.find(l,1101,found)&&found.serial==serial); // Duplicate native launch copies keep the same plan.
    cache.remember(s,2000);assert(!cache.find(l,122001,found));cache.remember(s,130000);assert(cache.find(l,130010,found)&&found.serial!=serial);cache.clear();assert(!cache.find(l,130011,found));
    attendance::State game;game.team0=1153;game.team1=1193;game.startingAttendance=true;attendance::arrivals::Settings arrivals;
    assert(attendance::arrivals::openingPercent(arrivals,game)==90);game.night=true;assert(attendance::arrivals::openingPercent(arrivals,game)==97);
    game.team0=1212;assert(attendance::arrivals::openingPercent(arrivals,game)==88);game.night=false;assert(attendance::arrivals::openingPercent(arrivals,game)==78);
    game.playoff=true;auto protectedGame=game;protectedGame.startingAttendance=false;assert(attendance::arrivals::openingPercent(arrivals,game)==attendance::arrivals::openingPercent(arrivals,protectedGame));
    game.playoff=false;game.conferenceChampionship=true;protectedGame=game;protectedGame.startingAttendance=false;assert(attendance::arrivals::openingPercent(arrivals,game)==attendance::arrivals::openingPercent(arrivals,protectedGame));
    std::puts("PASS: turnout demand/form/night/weather, 19:00 boundary, neutral symmetry, FCS, protected/resumed fallback, fixture stability, stale/cross-game rejection and revised-only arrivals.");
}
