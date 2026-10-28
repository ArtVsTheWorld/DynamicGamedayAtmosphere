#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace attendance::turnout {
// Expected settled turnout, not the number already in their seats at kickoff.
// Inputs are frozen at launch. All adjustments below are capacity percentage points.
struct Team {
    int id=-1,rank=0,toughest=0,prestige=0,wins=0,losses=0,homeGames=0;
};
struct Seed {
    Team home,away;
    int stadium=-1,homeStadium=-1,homeAlternate=-1,capacity=0,schools=0,week=-1,year=0,ea=0,roof=0;
    bool regular=false,resumed=false;
    uint64_t serial=0,observed=0;
};
struct Launch {
    int home=-1,away=-1,stadium=-1,capacity=0,ea=0,minutes=-1,precipitation=-1;
    bool rivalry=false,neutral=false;
};
struct Result {
    bool applied=false,night=false;
    int attendance=0,kickoff=90;
    float demand=0,momentum=0,appeal=0,nightBonus=0,weather=0,variation=0,limits=0,percent=0;
};
inline bool lateArrivalSchool(int team){return team==1212||team==1158||team==1137||team==1156;}
inline int kickoffPercent(int home,bool neutral,bool night){return !neutral&&lateArrivalSchool(home)?(night?88:78):(night?97:90);}
inline bool validTeam(const Team& t,int schools){
    return t.id>=0&&t.id<2047&&t.toughest>=0&&t.toughest<=schools&&
        t.rank>=0&&t.rank<=256&&t.prestige>=0&&t.prestige<=10&&
        t.wins>=0&&t.losses>=0&&t.wins+t.losses<=30&&t.homeGames>=0&&t.homeGames<=20;
}
inline float demand(const Team& t,int schools){
    if(!t.toughest)return 45.f+5.f*t.prestige; // Missing atmosphere rank uses program support, not a fictional top rank.
    float loyalty=1.f-static_cast<float>(t.toughest-1)/(schools-1);
    float stadium=45.f+50.f*std::pow((std::clamp)(loyalty,0.f,1.f),.65f);
    return .8f*stadium+.2f*(45.f+5.f*t.prestige);
}
inline float momentum(const Team& t){
    int games=t.wins+t.losses;if(!games)return 0;
    float confidence=(std::min)(1.f,games/6.f);
    return (std::clamp)((t.wins/static_cast<float>(games)-.5f)*20.f*confidence,-10.f,8.f);
}
inline float opponent(const Team& t){
    if(t.id>=1500&&t.id<=1504)return -8;
    if(t.rank>=1&&t.rank<=25)return 2.f+6.f*(26-t.rank)/25.f;
    if(t.rank>100)return -4;
    return 0;
}
inline uint32_t fixtureHash(const Seed& s,bool neutral=false){
    uint32_t h=2166136261u;
    int a=s.home.id,b=s.away.id;if(neutral&&a>b)std::swap(a,b);
    for(int n:{a,b,s.stadium,s.week,s.year}){h^=static_cast<uint32_t>(n);h*=16777619u;}
    return h;
}
inline Result calculate(const Seed& s,const Launch& l){
    Result r;r.attendance=l.ea;
    if(!s.regular||s.resumed||s.week<0||s.week>31||s.year<0||s.year>4095||s.schools<100||s.schools>512||!validTeam(s.home,s.schools)||!validTeam(s.away,s.schools)||
       s.home.id==s.away.id||l.home!=s.home.id||l.away!=s.away.id||l.stadium!=s.stadium||l.ea!=s.ea||
       l.capacity<1000||l.capacity>200000||l.ea<1||l.ea>l.capacity*2||l.minutes<0||l.minutes>=1440||l.precipitation<0||l.precipitation>3)return r;
    r.night=l.minutes>=1140; // Verified FUNCTIONKEY_ISNIGHTGAME: stadium-local kickoff >=19:00.
    r.demand=demand(s.home,s.schools);r.momentum=momentum(s.home);
    r.appeal=opponent(s.away)+(l.rivalry?10.f:0.f)+(s.home.homeGames==0?4.f:0.f);
    if(l.neutral){
        r.demand=.5f*(r.demand+demand(s.away,s.schools));
        r.momentum=.5f*(r.momentum+momentum(s.away));
        r.appeal=.5f*(opponent(s.home)+opponent(s.away))+(l.rivalry?10.f:0.f);
    }
    r.appeal=(std::clamp)(r.appeal,-8.f,15.f);
    r.nightBonus=r.night?7.f:0.f;
    const float weather[]={0.f,5.f,9.f,14.f};r.weather=s.roof>=1&&s.roof<=3?0.f:-weather[l.precipitation];
    r.variation=static_cast<float>(static_cast<int>(fixtureHash(s,l.neutral)%7)-3);
    float raw=r.demand+r.momentum+r.appeal+r.nightBonus+r.weather+r.variation;
    r.percent=(std::clamp)(raw,30.f,100.f);r.limits=r.percent-raw;
    r.attendance=(std::clamp)(static_cast<int>(std::lround(l.capacity*r.percent*.01f)),1,l.capacity);
    r.kickoff=kickoffPercent(s.home.id,l.neutral,r.night);r.applied=true;return r;
}
// Server simulation calls can interleave with a playable launch. Match the actual
// launch IDs, native count and stadium; never use a reused match pointer as identity.
class Seeds {
    std::vector<Seed> recent_;uint64_t serial_=0;
public:
    void remember(Seed s,uint64_t now){
        s.serial=++serial_;s.observed=now;recent_.push_back(s);
        if(recent_.size()>32)recent_.erase(recent_.begin());
    }
    void clear(){recent_.clear();}
    bool find(const Launch& l,uint64_t now,Seed& out)const{
        for(auto p=recent_.rbegin();p!=recent_.rend();++p){
            if(p->home.id!=l.home||p->away.id!=l.away||p->stadium!=l.stadium||p->ea!=l.ea)continue;
            if(now<p->observed||now-p->observed>120000)return false;
            out=*p;return true; // Native loading may copy the same launch options twice.
        }
        return false;
    }
};
struct Applied {
    Seed seed;Launch launch;Result result;
    bool owns(int home,int away)const{return result.applied&&launch.home==home&&launch.away==away;}
};
}
