#pragma once
#include "ExcitementRules.h"

namespace attendance::lineup {
// Observe the opening possession independently of audio eligibility. Reaching
// the fullness gate later must not turn the second drive into an opening drive.
struct OpeningDrive {
    bool seen=false,armed=false,started=false,closed=false,finishing=false;
    int offense=-1,home=0,away=0;float highTime=0;
    int firstOffense=-1;bool firstSnapSeen=false,firstSnapEnded=false;
    bool awayOpeningPlay()const{return armed&&!closed&&firstOffense==1&&!firstSnapEnded;}
    bool observe(const energy::Play& p,const energy::Outcome* result=nullptr){
        const auto& s=p.game;
        if(!valid(s))return false;
        if(p.resumed||p.simulated||s.quarter>=3||s.phase==7){closed=true;return false;}
        float time=elapsedGameTime(s);
        if(seen&&time+.01f<highTime)return false; // A rewind cannot create a fresh drive.
        highTime=(std::max)(highTime,time);
        if(!seen){
            seen=true;armed=s.quarter==1&&!s.homeScore&&!s.awayScore&&s.seconds>=s.quarterSeconds-1&&
                (s.phase==6||s.phase==1||s.phase==5);
            home=s.homeScore;away=s.awayScore;
        }
        if(!armed||closed)return false;
        bool selection=s.phase==1||s.phase==5;
        if(firstSnapSeen&&s.phase!=3&&(selection||s.phase==2||s.phase==4))firstSnapEnded=true;
        if(firstOffense<0&&energy::normalDown(p)&&((selection&&!p.specialTeams)||s.phase==3))firstOffense=p.offense;
        if(s.phase==3&&energy::normalDown(p)&&firstOffense>=0)firstSnapSeen=true;
        bool score=s.homeScore!=home||s.awayScore!=away;
        bool endResult=result&&result->accepted&&result->completed&&(result->homePoints||result->awayPoints||result->takeaway);
        bool changed=started&&(p.offense==0||p.offense==1)&&p.offense!=offense&&energy::normalDown(p);
        // Keep the last play's eligibility through a live turnover return and
        // its whistle. The next selection ends this allowance, not the catch.
        if(score||endResult||changed)finishing=true;
        if(finishing&&selection){closed=true;return false;}
        if(!started&&score){closed=true;return false;} // Opening return TD/safety.
        if(!started&&energy::normalDown(p)&&!p.specialTeams&&(selection||s.phase==3)){
            started=true;offense=p.offense;
        }
        return started&&!closed;
    }
};
}
