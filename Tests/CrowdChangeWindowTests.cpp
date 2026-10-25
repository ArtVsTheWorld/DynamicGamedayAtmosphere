#include "../Source/CrowdChangeWindow.h"
#include "../Source/ArrivalRules.h"
#include "../Source/DepartureModel.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    CrowdChangeWindow window;arrivals::Model arrivals;arrivals::Settings arriving;
    arriving.startPercent=60;arriving.finishPercent=50; // Exercise a long custom arrival window.
    State s{1,0,0,6,900,900};
    auto entry=[&](){window.observe(123,s);return window.take(123,s);};
    assert(entry());assert(arrivals.observe(arriving,s).missing==40);
    for(int i=0;i<20;++i)assert(!entry()); // Same intro never opens a second window.
    s.phase=1;
    for(int seconds=900;seconds>=700;seconds-=5){s.seconds=static_cast<float>(seconds);assert(!entry());assert(arrivals.missing==40);}
    s.phase=3;s.seconds=680;assert(!entry());s.phase=4;s.seconds=670;assert(!entry());
    s.phase=5;s.seconds=650;assert(entry());assert(arrivals.observe(arriving,s).missing==18);
    for(int seconds=645;seconds>=550;seconds-=5){s.seconds=static_cast<float>(seconds);assert(!entry());assert(arrivals.missing==18);}
    s.phase=1;s.seconds=540;assert(!entry());s.phase=3;s.seconds=530;assert(!entry());
    s.phase=5;s.seconds=500;window.observe(123,s); // A throttled decision can consume the pending entry later in selection.
    assert(window.take(123,s));assert(!window.take(123,s));assert(arrivals.observe(arriving,s).missing==5);
    s.phase=3;s.seconds=480;window.observe(123,s);s.phase=5;s.seconds=470;window.observe(123,s);
    s.phase=1;s.seconds=460;window.observe(123,s);assert(!window.take(123,s)); // Missed brief selection must not settle at the line.
    s.phase=5;s.seconds=440;assert(entry());assert(arrivals.observe(arriving,s).finished);
    s.seconds=600;assert(!entry()); // Replay rewind cannot open a new visual window.
    window.reset();departures::Model model;Settings exits;History history;
    s={4,0,35,5,900,900};int loss=50;assert(entry());model.observe(exits,s,history);
    s.phase=1;
    for(int seconds=895;seconds>=600;seconds-=5){s.seconds=static_cast<float>(seconds);model.observe(exits,s,history);assert(!entry());assert(loss==50);}
    s.phase=3;s.seconds=590;assert(!entry());s.phase=4;s.seconds=580;assert(!entry());
    s.phase=5;s.seconds=570;assert(entry());model.observe(exits,s,history);loss=model.settle(s,loss).loss;assert(loss>50);int first=loss;
    s.seconds=500;model.observe(exits,s,history);assert(!entry());assert(loss==first); // Repeated selection frames do not apply accumulated exits.
    s.phase=1;s.seconds=490;assert(!entry());s.phase=3;s.seconds=480;assert(!entry());
    s.phase=5;s.seconds=450;assert(entry());model.observe(exits,s,history);loss=model.settle(s,loss).loss;assert(loss>=first);
    s.quarter=5;s.seconds=900;assert(entry()); // A real new quarter is a break even if the phase stays PlayCall.
    window.observe(456,State{1,0,0,6,900,900});assert(window.take(456,State{1,0,0,6,900,900}));
    std::puts("PASS: arrivals/exits stay fixed at the line, during snaps and repeated selection frames; only selection entries, initial pregame and real breaks settle changes; throttling and replay guards.");
}
