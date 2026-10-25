#include "../Source/DepartureModel.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace attendance;
struct Run {int q2=0,half=0,q4=0,warning=0,final=0;double hazard=0;};
Run simulate(State s,int step=5,Settings c={}){
    departures::Model model;History h;Run r;int loss=0;
    int q=static_cast<int>(s.quarterSeconds);
    for(int quarter=1;quarter<=4;++quarter){
        s.quarter=quarter;s.phase=5;
        for(int seconds=q;seconds>=0;seconds=(std::max)(-1,seconds-step)){
            s.seconds=static_cast<float>(seconds);assert(h.observe(c,s));assert(model.observe(c,s,h));
            int before=loss;loss=model.settle(s,loss,0,c.quarterCap).loss;assert(loss>=before&&loss<=lossCeiling(s));
            if(seconds==q){if(quarter==2)r.q2=loss;if(quarter==3)r.half=loss;if(quarter==4)r.q4=loss;}
            if(quarter==4&&seconds==120)r.warning=loss;
        }
    }
    r.final=loss;r.hazard=model.hazard;return r;
}
int main(){
    Settings c;State s{1,0,0,5,900,900};History h;departures::Model m;
    // An independent analytical oracle for constant routine pressure.
    Run close=simulate(s);assert(close.hazard>0.10&&close.final>=10);
    assert(close.q2==0&&close.half>=10);
    s.awayScore=70;Run blowout=simulate(s);
    assert(blowout.final==75);
    assert(blowout.half>blowout.q2+20&&blowout.q4>blowout.half);
    s.precipitation=1;assert(simulate(s).final<=85&&simulate(s).final>=70);
    for(int kind:{1,2})for(int weather:{0,1,2}){
        s.playoff=true;s.bowl=kind==2?47:37;s.precipitation=weather;
        auto r=simulate(s);assert(r.final<=lossCeiling(s)&&r.final>=lossCeiling(s)-12);
        s.awayScore=21;assert(simulate(s).final>0&&simulate(s).final<(kind==1?15:10));s.awayScore=70;
    }
    s={1,0,21,5,720,720};Run ordinary=simulate(s);
    assert(ordinary.final>=40&&ordinary.final<=60&&ordinary.half>=20&&ordinary.half<=35);
    assert(ordinary.q4-ordinary.half<10&&ordinary.final-ordinary.q4>ordinary.q4-ordinary.half);
    State homeWin=s;homeWin.homeScore=35;homeWin.awayScore=0;Run winning=simulate(homeWin);
    assert(winning.final<=65&&winning.q4-winning.half<=15);
    homeWin.rank0=1;homeWin.rank1=127;assert(simulate(homeWin).final>=winning.final);
    s.homeScore=21;s.awayScore=0;assert(simulate(s).final<ordinary.final);
    s=reversed(s);s.rivalry=true;Run rival=simulate(s);assert(rival.final<ordinary.final);
    s.precipitation=1;assert(simulate(s).final==rival.final); // Matchup precedence, while the floor can differ.
    s.rivalry=false;assert(simulate(s).final>ordinary.final);
    s.precipitation=0;s.rank0=50;s.rank1=1;Run mismatch=simulate(s);
    s.rank0=127;assert(simulate(s).final>mismatch.final);
    float previous=1;
    for(int rank=2;rank<=255;++rank){s.rank0=rank;float next=departures::rankMultiplier(s);assert(next>previous);previous=next;}
    s.rank0=0;s.team0=1500;assert(departures::rankMultiplier(s)>previous);
    s.team0=-1;assert(departures::rankMultiplier(s)==1); // Unknown FBS is not made up.
    // Neutral sites follow either team's story identically.
    s={1,0,21,5,900,900};s.neutral=true;s.rank0=127;s.rank1=1;
    assert(simulate(s).final==simulate(reversed(s)).final);
    s.homeScore=21;s.awayScore=14;h.observe(c,s);h.watchStage=3;
    float massive=departures::stayFactor(s,h);s.rank0=26;assert(departures::stayFactor(s,h)>massive);
    h.comeback=true;assert(departures::rate(c,s,h,3000)==0);
    // No historical departures on late attach; pauses and score/clock rewinds
    // neither consume opportunities nor poison the accepted high-water state.
    m.reset();h.reset();s={3,0,35,5,450,900};assert(m.observe(c,s,h));assert(m.settle(s,0).loss==0);
    for(int i=0;i<500;++i)assert(m.observe(c,s,h));assert(m.hazard==0);
    s.seconds=460;assert(!m.observe(c,s,h));s.seconds=440;s.awayScore=28;assert(!m.observe(c,s,h));
    s.awayScore=35;assert(m.observe(c,s,h)&&m.hazard>0);
    double once=m.hazard;assert(m.observe(c,s,h)&&m.hazard==once);
    // A newly developed blowout cannot charge its higher rate to earlier time.
    m.reset();s={2,0,0,5,900,900};m.observe(c,s,h);s.awayScore=35;s.seconds=450;m.observe(c,s,h);
    assert(std::abs(m.hazard+std::log(0.95))<0.00001); // Only routine pressure in the preceding half-quarter.
    double before=m.hazard;s.seconds=400;m.observe(c,s,h);assert(m.hazard>before+0.008);
    // Protection and disabling do not bank exits for a later catch-up.
    for(bool manual:{false,true}){
        m.reset();s={2,0,35,5,900,900};m.observe(c,s,h);
        h.comeback=!manual;s.seconds=600;m.observe(c,s,h,!manual);
        h.comeback=false;s.seconds=300;m.observe(c,s,h);assert(m.hazard==0);
        s.seconds=200;m.observe(c,s,h);assert(m.hazard>0);
    }
    m.reset();s={2,0,35,5,900,900};m.observe(c,s,h);
    s.seconds=450;m.observe(c,s,h);assert(m.hazard>0);
    h.comeback=true;m.observe(c,s,h);assert(m.hazard==0&&m.settle(s,0).loss==0);
    s.seconds=300;m.observe(c,s,h);h.comeback=false;s.seconds=250;m.observe(c,s,h);assert(m.hazard==0);
    // External late-score losses join the accumulated history without refill.
    m.reset();s={4,0,35,5,300,900};m.observe(c,s,h);assert(m.settle(s,40).loss==40);
    assert(m.settle(s,0).loss==40);s.homeScore=35;s.seconds=290;m.observe(c,s,h);assert(m.settle(s,40).loss>=40);
    m.reset();s={4,0,35,5,900,900};m.observe(c,s,h);m.settle(s,40);
    s.seconds=600;m.observe(c,s,h);double queued=m.hazard-m.committedHazard;assert(queued>0);
    int combined=m.settle(s,45).loss;assert(combined>45);
    assert(m.settle(s,combined).loss==combined); // External + ordinary exits applied once.
    m.reset();s.precipitation=1;assert(m.settle(s,90).loss==90);
    s.precipitation=0;assert(m.settle(s,80).loss==90); // A higher floor cannot refill earlier exits.
    s.quarter=5;s.homeScore=s.awayScore=35;s.seconds=0;m.observe(c,s,h);before=m.hazard;
    s.seconds=900;m.observe(c,s,h);assert(m.hazard==before&&m.settle(s,40).loss>=40);
    // Quarter and warning charges are independent of callback frequency.
    s={1,0,21,5,900,900};auto fine=simulate(s,1),coarse=simulate(s,30);
    assert(std::abs(fine.hazard-coarse.hazard)<0.0001&&fine.final==coarse.final);
    for(int minutes=1;minutes<=20;++minutes){
        s.quarterSeconds=static_cast<float>(minutes*60);s.seconds=s.quarterSeconds;s.awayScore=0;
        auto r=simulate(s,1);assert(std::isfinite(r.hazard)&&r.half>=10&&r.final>=10);
        s.awayScore=21;r=simulate(s,1);
        assert(r.half>=20&&r.half<=35&&r.final>=40&&r.final<=60);
        assert(r.q4-r.half<10&&r.final-r.q4>r.q4-r.half);
        s.homeScore=35;s.awayScore=0;r=simulate(s,1);assert(r.final<=65);
        s.homeScore=0;
    }
    c.warningEnabled=0;s={1,0,0,5,900,900};assert(simulate(s,5,c).half>=10);
    s={1,17,17,5,720,720};assert(simulate(s).half<10);
    s={1,0,0,5,720,720};s.rivalry=true;assert(simulate(s).half<10);
    s.rivalry=false;s.rank0=6;s.rank1=11;assert(simulate(s).half<10);
    // Cap cumulative original-crowd percentage points, including late-score extras.
    m.reset();h.reset();s={2,0,70,5,720,720};m.observe(c,s,h);int capped=0;
    for(int clock=715;clock>0;clock-=5){s.seconds=static_cast<float>(clock);m.observe(c,s,h);capped=m.settle(s,capped,0,30).loss;assert(capped<=30);}
    auto extra=m.settle(s,capped,5,30);assert(extra.loss<=30);
    s.quarter=3;s.seconds=720;m.observe(c,s,h);int halfLoss=m.settle(s,capped,0,30).loss;assert(halfLoss-capped<=30);
    s.seconds=715;m.observe(c,s,h);assert(m.settle(s,halfLoss).loss-halfLoss<=1); // Blocked debt is discarded.
    State inferior{3,35,0,5,720,720};inferior.rank0=1;inferior.rank1=127;
    State equal=inferior;equal.rank1=2;assert(inferiorOpponent(inferior)==1&&inferiorOpponent(equal)==0);
    assert(departures::rate(c,inferior,h,1800)>departures::rate(c,equal,h,1800));
    inferior.rank1=0;inferior.team1=1500;assert(inferiorOpponent(inferior)==1);
    assert(c.quarterCap==45);
    for(int rank:{1,25})for(int weather:{0,1,2})for(bool fcs:{false,true}){
        State winner{1,70,0,5,720,720};winner.rank0=rank;winner.rank1=fcs?0:127;winner.team1=fcs?1500:1158;winner.precipitation=weather;
        assert(simulate(winner).final==65); // Severe mismatch still retains 35%.
        m.reset();h.reset();winner.quarter=4;winner.seconds=100;winner.homeScore=0;winner.awayScore=35;
        m.observe(c,winner,h);assert(m.settle(winner,70).loss==70);
        winner.homeScore=42;winner.awayScore=35;winner.seconds=90;
        h.observe(c,winner);m.observe(c,winner,h);assert(m.settle(winner,70).loss==70); // No refill after taking the lead.
        m.reset();winner.homeScore=70;winner.awayScore=0;m.observe(c,winner,h);m.hazard=10;
        assert(m.settle(winner,65,25).loss==65); // Late-score extras share the hard floor.
    }
    for(int kind:{0,1,2}){
        m.reset();s={2,0,70,5,720,720};s.playoff=kind!=0;s.bowl=kind==2?47:37;
        m.observe(c,s,h);m.hazard=10;int limit=kind==0?45:kind==1?20:10;
        auto cap=m.settle(s,0,5);assert(cap.loss==limit&&m.settle(s,cap.loss,5).loss==limit);
    }
    c.enabled=0;assert(simulate(s,5,c).final==0);
    std::cout<<"PASS: analytic formula, distributed exits, score/time/home/rank/weather context, FCS ordering, all floors, postseason gating, replay/late attach, protection, OT, no refill and callback invariance.\n";
}
