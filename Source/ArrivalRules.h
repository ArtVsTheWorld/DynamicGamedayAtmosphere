#pragma once
#include "AttendanceRules.h"
namespace attendance::arrivals {
struct Settings {
    int enabled=1,startPercent=65,lateStartPercent=45;
    int finishPercent=25,lateFinishPercent=75;
    int bigGameBonus=20,badWeatherPenalty=10,playoffStart=65,championshipStart=75;
    int fcsPenalty=10,moderateStartPercent=55,moderateFinishPercent=50;
};
enum class Group { Standard,Moderate,Severe };
// FootballStaticTeams_DBE: IDs verified from the locally generated game SDK.
inline bool fcsTeam(int team){return attendance::fcsTeam(team);}
inline bool fcsOpponent(const State& s){return fcsTeam(s.team1)||(s.neutral&&fcsTeam(s.team0));}
inline Group schoolGroup(const State& s){
    if(s.neutral)return Group::Standard;
    switch(s.team0){
    case 1212:case 1158:case 1137:case 1156:return Group::Severe; // UCLA, Miami, GT, Maryland.
    case 1216:case 1189:case 1113:case 1117:case 1197:case 1182:case 1154:case 1132:return Group::Moderate;
    default:return Group::Standard;
    }
}
inline bool lateSchool(const State& s){return schoolGroup(s)==Group::Severe;}
inline float adjustment(const Settings& c,const State& raw){
    State s=ranked(raw);float importance=s.rivalry?1.0f:0.0f;
    float gap=relativeGap(s),underdog=0;
    if(s.rank0>0&&s.rank1>0){
        float homeQuality=std::exp(-(s.rank0-1)/35.0f),awayQuality=std::exp(-(s.rank1-1)/35.0f);
        // A strong visiting opponent attracts interest even when only one team
        // is ranked. Neutral sites use the same calculation for either label.
        float opponent=s.neutral?(std::max)(homeQuality,awayQuality):awayQuality;
        importance=(std::max)(importance,0.6f*std::sqrt(homeQuality*awayQuality)+0.4f*opponent);
        if(!s.neutral&&s.rank0>s.rank1)underdog=gap;
    }else{
        int opponent=s.neutral?(std::max)(s.rank0,s.rank1):s.rank1;
        if(opponent>0)importance=(std::max)(importance,0.4f*std::exp(-(opponent-1)/35.0f));
    }
    if(s.playoff)importance=(std::max)(importance,championship(s)?1.0f:0.85f);
    float bonus=(std::clamp)(c.bigGameBonus,0,20)*importance;
    float penalty=(badWeather(s)?(std::clamp)(c.badWeatherPenalty,0,30):0)+3*gap+5*underdog;
    if(!s.playoff&&fcsOpponent(s))penalty+=(std::clamp)(c.fcsPenalty,0,30);
    float adjusted=(std::clamp)(bonus-penalty,-20.0f,20.0f);
    return adjusted-(!s.playoff&&fcsOpponent(s)?5.0f:0.0f);
}
inline int baselinePercent(const Settings& c,const State& s){
    return championship(s)?c.championshipStart:s.playoff?c.playoffStart:
        schoolGroup(s)==Group::Severe?c.lateStartPercent:schoolGroup(s)==Group::Moderate?c.moderateStartPercent:c.startPercent;
}
inline int openingPercent(const Settings& c,const State& s){
    return (std::clamp)(static_cast<int>(std::lround(baselinePercent(c,s)+adjustment(c,s))),10,100);
}
inline float finishFraction(const Settings& c,const State& s){
    int base=schoolGroup(s)==Group::Severe?c.lateFinishPercent:schoolGroup(s)==Group::Moderate?c.moderateFinishPercent:c.finishPercent;
    return (std::clamp)((std::clamp)(base,10,100)*0.01f*std::exp(-adjustment(c,s)/40.0f),0.10f,0.98f);
}
struct Change {bool changed=false,starting=false,finished=false;int missing=0;};
struct Model {
    bool seen=false,active=false,done=false;int missing=0,startMissing=0;float elapsed=0,fillSeconds=0;
    void reset(){*this=Model{};}
    Change finish(){Change r;r.changed=missing>0;r.finished=active;r.missing=0;missing=0;active=false;done=true;return r;}
    Change observe(const Settings& c,const State& s){
        Change r;r.missing=missing;
        if(!valid(s)||(s.phase!=1&&s.phase!=5&&s.phase!=6))return r;
        if(done)return r;
        if(!c.enabled||s.quarter>1)return finish();
        if(!seen){
            seen=true;
            // An update loaded halfway through a game must never empty seats
            // to simulate a kickoff that already happened.
            if(s.quarter!=1||s.seconds<s.quarterSeconds*0.95f||s.homeScore||s.awayScore)return finish();
            int start=openingPercent(c,s);
            startMissing=100-start;missing=startMissing;
            fillSeconds=s.quarterSeconds*finishFraction(c,s);
            active=missing>0;done=!active;r.starting=active;r.changed=active;r.missing=missing;
        }
        float now=s.quarterSeconds-s.seconds;
        if(now+0.01f<elapsed)return r;
        elapsed=now;
        int next=static_cast<int>(std::ceil(startMissing*(1-elapsed/fillSeconds)));
        next=(std::clamp)(next,0,missing);
        r.changed=r.changed||next!=missing;missing=next;r.missing=missing;
        if(missing==0&&active){active=false;done=true;r.finished=true;}
        return r;
    }
};
// These are temporarily absent fans, separate from permanent departures. Only
// plugin-owned seats may return. Game-clock high water prevents replay cycling.
struct Halftime {
    bool seen=false,active=false,done=false;float highTime=0;int temporary=0;
    void reset(){*this=Halftime{};}
    int observe(const State& s,int permanent,int extra=10){
        if(!valid(s)||(s.phase!=1&&s.phase!=5)||done)return 0;
        float now=(s.quarter-1)*s.quarterSeconds+s.quarterSeconds-s.seconds;
        if(seen&&now+0.01f<highTime)return temporary;
        seen=true;highTime=now;
        if(extra<=0){active=false;temporary=0;return 0;}
        const float before=s.quarterSeconds/8.0f,after=s.quarterSeconds/6.0f;
        if(s.quarter>3||(s.quarter==3&&s.quarterSeconds-s.seconds>=after)){done=true;active=false;temporary=0;return 0;}
        if(s.quarter==2&&s.seconds<=before)active=true;
        // Smooth both directions. A late attach in Q3 never invents a seat break.
        float amount=s.quarter==2?1-s.seconds/before:1-(s.quarterSeconds-s.seconds)/after;
        amount=(std::clamp)(amount,0.0f,1.0f);amount=amount*amount*(3-2*amount);
        int missing=static_cast<int>(std::lround((std::clamp)(extra,0,10)*amount));
        temporary=active?(std::max)(permanent,(std::min)(lossCeiling(s),permanent+missing)):0;
        return temporary;
    }
};
}
