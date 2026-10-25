#include "../Source/ArrivalRules.h"
#include "../Source/CrowdVisibility.h"
#include "../Source/MatchLifecycle.h"
#include <cassert>
#include <vector>
#include <iostream>
using namespace attendance;
int main(){
    arrivals::Settings c;arrivals::Model m;State s{1,0,0,6,720,720};
    const int severe[]={1212,1158,1137,1156};
    const int moderate[]={1216,1189,1113,1117,1197,1182,1154,1132};
    for(int team:severe){s.team0=team;assert(arrivals::schoolGroup(s)==arrivals::Group::Severe&&arrivals::openingPercent(c,s)==50&&arrivals::finishFraction(c,s)==0.75f);}
    for(int team:moderate){s.team0=team;assert(arrivals::schoolGroup(s)==arrivals::Group::Moderate&&arrivals::openingPercent(c,s)==60&&arrivals::finishFraction(c,s)==0.50f);}
    for(int team:{-1,0,1159,1136,1211}){s.team0=team;assert(arrivals::schoolGroup(s)==arrivals::Group::Standard&&arrivals::openingPercent(c,s)==70&&arrivals::finishFraction(c,s)==0.25f);}
    for(int team:{1212,1216,1158,1154}){s.team0=team;s.neutral=true;assert(arrivals::openingPercent(c,s)==70&&arrivals::finishFraction(c,s)==0.25f);}s.neutral=false;
    // Every supported integer quarter length, including the 12-minute reference.
    for(int minutes=1;minutes<=20;++minutes)for(int team:{1212,1154,-1}){
        m.reset();s={1,0,0,6,static_cast<float>(minutes*60),static_cast<float>(minutes*60)};s.team0=team;
        int start=team==1212?50:team==1154?60:70;auto r=m.observe(c,s);assert(r.starting&&r.missing==100-start);
        float finish=m.fillSeconds;s.phase=1;s.seconds=s.quarterSeconds-finish/2;
        int halfMissing=static_cast<int>(std::ceil((100-start)*std::pow(0.5,1.6)));
        for(int i=0;i<20;++i)assert(m.observe(c,s).missing==halfMissing);
        s.seconds=s.quarterSeconds;assert(!m.observe(c,s).changed); // Rewinds do not reverse progress.
        s.seconds=(std::max)(0.0f,s.quarterSeconds-finish-0.1f);assert(m.observe(c,s).finished&&m.missing==0);
        assert(!m.observe(c,s).changed&&m.done);
    }
    // Formula inputs affect the start and fill time together, with hard +20/-20 limits.
    for(int team:{-1,1212,1154})for(int weather:{0,1,2})for(int homeRank:{0,1,6,25,50,127,255})for(int awayRank:{0,1,11,25,50,127,255})for(bool rival:{false,true}){
        s={1,0,0,6,720,720};s.team0=team;s.precipitation=weather;s.rank0=homeRank;s.rank1=awayRank;s.rivalry=rival;
        float delta=arrivals::adjustment(c,s),finish=arrivals::finishFraction(c,s);
        assert(std::isfinite(delta)&&delta>=-20&&delta<=20&&finish>=0.10f&&finish<=0.98f);
        assert(arrivals::openingPercent(c,s)>=arrivals::baselinePercent(c,s)-20&&arrivals::openingPercent(c,s)<=100);
        State wet=s;wet.precipitation=1;assert(arrivals::openingPercent(c,wet)<=arrivals::openingPercent(c,s));
        assert(arrivals::finishFraction(c,wet)>=finish-0.00001f);
    }
    s={1,0,0,6,720,720};float ordinary=arrivals::finishFraction(c,s);s.rivalry=true;
    assert(arrivals::openingPercent(c,s)==92&&arrivals::finishFraction(c,s)<ordinary);
    s.rivalry=false;s.precipitation=1;assert(arrivals::openingPercent(c,s)==60&&arrivals::finishFraction(c,s)>ordinary);
    s.precipitation=0;s.rank0=6;s.rank1=11;assert(arrivals::openingPercent(c,s)>70&&arrivals::finishFraction(c,s)<ordinary);
    s.rank0=127;s.rank1=1;float mismatchOpponent=arrivals::adjustment(c,s);s.rank1=50;assert(mismatchOpponent<arrivals::adjustment(c,s));
    s.rank0=1;s.rank1=50;float mismatch=arrivals::adjustment(c,s);s.rank1=127;assert(arrivals::adjustment(c,s)<mismatch);
    s.neutral=true;assert(arrivals::openingPercent(c,s)==arrivals::openingPercent(c,reversed(s))&&arrivals::finishFraction(c,s)==arrivals::finishFraction(c,reversed(s)));
    for(int fcs:{1500,1501,1502,1503,1504}){
        s={1,0,0,6,720,720};s.team1=fcs;s.precipitation=1;s.rank0=127;
        assert(arrivals::adjustment(c,s)>=-25&&arrivals::openingPercent(c,s)>=40);
        s.neutral=true;assert(arrivals::openingPercent(c,s)==arrivals::openingPercent(c,reversed(s)));
    }
    s={1,0,0,6,720,720};s.playoff=true;assert(arrivals::baselinePercent(c,s)==65&&arrivals::openingPercent(c,s)>=90);
    s.bowl=47;assert(arrivals::baselinePercent(c,s)==75&&arrivals::openingPercent(c,s)==95);
    m.reset();s={1,0,0,6,720,720};auto r=m.observe(c,s);assert(r.missing==30);
    s.phase=3;s.seconds=600;assert(!m.observe(c,s).changed&&m.missing==30);
    s.phase=5;r=m.observe(c,s);assert(r.missing==6);s.seconds=620;assert(!m.observe(c,s).changed);
    s.seconds=540;assert(m.observe(c,s).finished);
    m.reset();s={1,0,0,5,200,720};assert(!m.observe(c,s).changed&&m.done); // Late attach.
    m.reset();s={4,17,17,5,720,720};assert(!m.observe(c,s).changed&&m.done);
    m.reset();s={1,0,0,6,720,720};m.observe(c,s);assert(m.finish().finished&&m.missing==0);
    m.reset();c.enabled=0;assert(!m.observe(c,s).changed);c.enabled=1;
    m.reset();c.startPercent=100;assert(!m.observe(c,s).changed&&m.done);c.startPercent=70;
    s={0,0,0,6,0,720};assert(normalizePregame(s)&&s.quarter==1&&s.seconds==720&&valid(s));
    State invalid=s;invalid.quarter=2;assert(!normalizePregame(invalid));invalid=s;invalid.homeScore=7;assert(!normalizePregame(invalid));
    invalid=s;invalid.phase=5;assert(!normalizePregame(invalid));invalid=s;invalid.quarterSeconds=NAN;assert(!normalizePregame(invalid));
    MatchLifecycle lifecycle;assert(!lifecycle.needsReset(1,true));lifecycle.join(1,true);
    for(int i=0;i<100;i++){assert(!lifecycle.needsReset(1,true));lifecycle.join(1,true);assert(lifecycle.keepAtKickoff(1));}
    assert(!lifecycle.keepAtKickoff(2)&&lifecycle.needsReset(2,true));
    lifecycle.live();assert(!lifecycle.keepAtKickoff(1)&&!lifecycle.needsReset(1,false)&&lifecycle.needsReset(1,true));
    lifecycle.reset();lifecycle.join(1,true);lifecycle.leave();assert(lifecycle.needsReset(1,true)&&!lifecycle.keepAtKickoff(1));
    lifecycle.reset();lifecycle.join(2,false);assert(!lifecycle.keepAtKickoff(2));
    State intro{1,0,0,6,720,720};lifecycle.reset();lifecycle.join(10,true);
    assert(lifecycle.carryOpening(11,intro,true)&&lifecycle.keepAtKickoff(11));
    intro.phase=1;assert(lifecycle.carryOpening(12,intro,true)&&lifecycle.keepAtKickoff(12));
    assert(!lifecycle.carryOpening(13,intro,false));
    intro.seconds=710;assert(!lifecycle.carryOpening(13,intro,true));intro.seconds=720;
    lifecycle.live();assert(!lifecycle.carryOpening(13,intro,true));
    lifecycle.reset();lifecycle.join(10,true);lifecycle.leave();assert(!lifecycle.carryOpening(11,intro,true));
    lifecycle.reset();lifecycle.join(10,true);intro.homeScore=7;assert(!lifecycle.carryOpening(11,intro,true));
    // Newly allocated renderer masks can start hidden until the native input arrives.
    assert(!hidePart(false,false,true,false,false,false,false,true));
    assert(hidePart(true,true,true,false,false,false,false,true));
    assert(hidePart(false,false,true,true,false,false,false,true));
    arrivals::Halftime half;
    for(int minutes=1;minutes<=20;++minutes){
        half.reset();float q=static_cast<float>(minutes*60),before=q/6,after=q/4;
        s={2,14,7,5,before+0.1f,q};assert(half.observe(s,5)==0);
        s.seconds=before;assert(half.observe(s,5)==5&&half.active);
        s.seconds=before/2;assert(half.observe(s,5)==13);
        s.seconds=before;assert(half.observe(s,5)==13); // Replay cannot unwind the ramp.
        s.seconds=0;assert(half.observe(s,5)==21);
        s.quarter=3;s.seconds=q;assert(half.observe(s,42)==52);
        s.seconds=q-after/2;assert(half.observe(s,42)==47);
        s.seconds=q-after-0.1f;assert(half.observe(s,42)==0&&half.done);
        s.quarter=2;s.seconds=0;assert(half.observe(s,42)==0);
        half.reset();s={3,0,0,5,q,q};assert(half.observe(s,0)==0); // No invented earlier absences.
    }
    half.reset();s={2,0,49,5,0,720};assert(half.observe(s,90)==90);
    half.reset();s.playoff=true;s.bowl=47;assert(half.observe(s,20)==20);
    half.reset();s.precipitation=1;assert(half.observe(s,20)==30);
    half.reset();s={2,14,7,5,0,720};assert(half.observe(s,5,50)==13);
    assert(half.observe(s,5,0)==0&&!half.active);
    // Simulate a stadium that the GAME only filled to 8,000 of 10,000 seats.
    // Arrivals may reveal only seats this plugin temporarily withheld.
    std::vector<bool> original(10000),current(10000),owned(10000);
    for(unsigned i=0;i<10000;i++)original[i]=current[i]=i>=8000;
    unsigned previous=0;
    for(int missing:{15,14,12,10,8,6,4,2,0}){
        unsigned visible=0;
        for(unsigned i=0;i<10000;i++){
            bool hold=departed(i,missing),wasOwned=owned[i];
            current[i]=hidePart(original[i],original[i],current[i],hold,false,true,wasOwned);
            if(hold&&!original[i])owned[i]=true;else if(!hold)owned[i]=false;
            assert(!original[i]||current[i]);
            if(!current[i])++visible;
        }
        assert(visible>=previous&&visible<=8000);previous=visible;
    }
    assert(previous==8000);
    // Halftime return simulation: temporary 10-point absence, main departure,
    // then only those still-temporary seats refill. Original empties survive.
    auto visit=[&](int permanent,int temporary){
        unsigned visible=0;
        for(unsigned i=0;i<10000;i++){
            bool left=departed(i,permanent),held=departed(i,temporary),wasOwned=owned[i];
            current[i]=hidePart(original[i],original[i],current[i],left||held,false,true,wasOwned);
            if(held&&!left&&!original[i])owned[i]=true;else if(!held)owned[i]=false;
            if(original[i]||left)assert(current[i]);if(!current[i])++visible;
        }
        return visible;
    };
    auto early=visit(5,15),halftime=visit(42,52),returned=visit(42,0);
    assert(early<8000&&halftime<early&&returned>halftime&&returned<early);
    assert(visit(42,0)==returned);previous=returned;
    // Departures then take over permanently: no comeback refill.
    for(int loss:{1,8,30,70}){
        unsigned visible=0;
        for(unsigned i=0;i<10000;i++){
            bool before=current[i];current[i]=hidePart(original[i],original[i],current[i],departed(i,loss),false);
            assert(!before||current[i]);if(!current[i])++visible;
        }
        assert(visible<=previous);previous=visible;
    }
    assert(hidePart(false,false,true,false,false,true,false)); // Unowned hidden seats stay hidden.
    assert(hidePart(true,false,true,false,false,true,true)); // Native hidden state wins.
    assert(hidePart(false,true,true,false,false,true,true)); // Original empty seats stay empty.
    assert(hidePart(false,false,true,true,false,true,true)); // Permanent departures beat owned arrivals.
    std::cout<<"PASS: first-quarter arrivals, pauses/replays, late attaches, between-play updates, original crowd ceiling, owned visibility restoration and permanent departures.\n";
}
