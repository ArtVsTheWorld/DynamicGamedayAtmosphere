#pragma once
#include "AtmosphereRules.h"

namespace attendance::departures {
inline float pressure(const State& s,float timeLeft){
    return (std::clamp)((std::abs(s.homeScore-s.awayScore)-8)/(8+32*timeLeft),0.0f,1.0f);
}
inline float rankMultiplier(const State& raw){
    State s=ranked(raw);
    if(s.rank0<1||s.rank1<1||s.homeScore==s.awayScore)return 1;
    bool favoriteLeads=(s.homeScore>s.awayScore&&s.rank0<s.rank1)||(s.awayScore>s.homeScore&&s.rank1<s.rank0);
    return favoriteLeads?1+0.25f*relativeGap(s):1;
}
inline float stayFactor(const State& s,const History& h){
    if(competitiveOvertime(s))return 0;
    if(h.comeback)return inferiorOpponent(s);
    float stage=(std::clamp)(h.watchStage/3.0f,0.0f,1.0f);
    return 1-0.85f*upsetDiscrepancy(s)*stage;
}
inline bool excellentGame(const State& s,const History& h){
    int margin=std::abs(s.homeScore-s.awayScore);
    return inferiorOpponent(s)<1&&margin<=8&&(s.rivalry||(top25(s.rank0)&&top25(s.rank1))||h.upsetWatch||h.comeback||
        (margin<=3&&s.homeScore+s.awayScore>=28));
}
inline float rate(const Settings& c,const State& s,const History& h,float elapsed){
    if(!c.enabled||s.quarter>4||s.phase==7)return 0;
    float t=(std::clamp)(1-elapsed/(4*s.quarterSeconds),0.0f,1.0f);
    float p=pressure(s,t),keep=stayFactor(s,h);
    float strength=(std::clamp)(c.formulaStrength,0,200)*0.01f;
    if(s.playoff){
        float gate=(std::clamp)((std::abs(s.homeScore-s.awayScore)-8)/27.0f,0.0f,1.0f);
        float postseasonRate=-std::log(1-lossCeiling(s)*0.01f);
        float home=!s.neutral&&s.homeScore>s.awayScore?0.65f+0.35f*inferiorOpponent(s):1.0f;
        return postseasonRate*p*p*gate*home*keep*strength;
    }
    // Preserve the accepted halftime cut, then soften moderate deficits in Q3
    // and let exits build faster in Q4. Severe blowouts retain their full rate.
    float severity=(std::clamp)((std::abs(s.homeScore-s.awayScore)-21)/14.0f,0.0f,1.0f);
    float late=elapsed>2*s.quarterSeconds+0.001f?(elapsed<3*s.quarterSeconds?0.25f:0.50f):1.0f;
    late+=severity*(1-late);
    bool homeAhead=!s.neutral&&s.homeScore>s.awayScore;
    float home=homeAhead?(elapsed<2*s.quarterSeconds?0.375f:elapsed<3*s.quarterSeconds?0.35f:0.405f):1.0f;
    home+=inferiorOpponent(s)*(1-home);
    Profile profile=profileFor(c,s);
    float context=profile==Profile::Reduced?0.75f+inferiorOpponent(s)*(badWeather(s)?0.5f:0.25f):profile==Profile::BadWeather?1.25f:1.0f;
    // Routine departures remain small in close games; the extra patience
    // applies to blowout-driven exits rather than switching routine losses off.
    return (0.08f+2.4f*p*p*home*rankMultiplier(s)*late)*context*keep*strength;
}
struct Result {int loss=0;bool changed=false;};
// Accrue game-time pressure independently of rendering. settle() is called ONLY
// at existing verified selection/break windows, never by a running clock alone.
struct Model {
    bool seen=false,automatic=false;float elapsed=0,length=0,minimumStart=-1;double hazard=0,committedHazard=0;
    int committedLoss=0,capPeriod=0,periodStartLoss=0;State previous;History priorHistory;
    void reset(){*this=Model{};}
    static float clockWeight(float from,float to,float quarter){
        auto overlap=[&](float start,float end,float budget){
            return budget*(std::max)(0.0f,(std::min)(to,end)-(std::max)(from,start))/(end-start);
        };
        return overlap(quarter,2*quarter,0.20f)+overlap(2*quarter,3*quarter,0.20f)+overlap(3*quarter,4*quarter,0.20f);
    }
    bool observe(const Settings& c,const State& s,const History& h,bool enabled=true,bool authoritative=false){
        if(!valid(s)||(!authoritative&&s.phase!=1&&s.phase!=5&&s.phase!=6))return false;
        float now=elapsedGameTime(s);
        if(seen&&(now+0.01f<elapsed||s.homeScore<previous.homeScore||s.awayScore<previous.awayScore||s.quarterSeconds!=length))return false;
        if(!enabled||!c.enabled||!c.formulaStrength||stayFactor(s,h)<=0||s.quarter>4){
            // Protection cancels departures not yet shown; it cannot bank an
            // unseen batch to remove as soon as the comeback ends.
            hazard=(std::min)(hazard,committedHazard);
            minimumStart=now<2*s.quarterSeconds?(std::max)(now,s.quarterSeconds):-1;
        }
        if(!seen&&now<2*s.quarterSeconds)minimumStart=(std::max)(now,s.quarterSeconds);
        bool live=enabled&&c.enabled&&s.phase!=6&&s.quarter<=4;
        if(seen&&live&&(!automatic||stayFactor(previous,priorHistory)<=0))minimumStart=now<2*length?(std::max)(now,length):-1;
        if(seen&&live&&automatic){
            // Integrate in small game-time steps. Both score/history endpoints
            // must permit exits: a new blowout or expired protection is never
            // backdated over an unobserved interval or a Super Sim jump.
            float step=(std::max)(0.25f,length/180.0f);
            for(float a=elapsed;a<now;){
                float b=(std::min)(now,a+step),mid=(a+b)*0.5f;
                float r=(std::min)(rate(c,previous,priorHistory,mid),rate(c,s,h,mid));
                hazard+=clockWeight(a,b,length)*r;a=b;
            }
            const float points[]={length,2*length,3*length};
            const float weights[]={0.02f,0.32f,0.04f};
            for(int i=0;i<3;++i)if(elapsed<points[i]&&now>=points[i])hazard+=weights[i]*rate(c,s,h,points[i]);
            float warning=4*length-120;
            if(c.warningEnabled&&length>120&&elapsed<warning&&now>=warning)hazard+=0.02f*rate(c,s,h,warning);
            if(!s.playoff&&minimumStart>=0&&elapsed<2*length&&now>minimumStart&&stayFactor(s,h)>0&&!excellentGame(s,h)&&c.formulaStrength>0){
                double progress=(std::clamp)((now-minimumStart)/(2*length-minimumStart),0.0f,1.0f);
                double minimum=0.10*progress*(std::clamp)(c.formulaStrength,0,200)/100.0;
                hazard=(std::max)(hazard,-std::log(1-minimum));
            }
        }
        // Pregame seeds Q1 without banking departures. Late attaches never
        // invent earlier checkpoints; disabled/manual time is not caught up.
        seen=true;elapsed=now;length=s.quarterSeconds;previous=s;priorHistory=h;
        automatic=enabled&&c.enabled&&s.quarter<=4;
        return true;
    }
    Result settle(const State& s,int currentLoss,int extra=0,int maximumPerQuarter=45){
        Result r;r.loss=currentLoss;
        if(!valid(s))return r;
        const int before=currentLoss;currentLoss=(std::max)(currentLoss,committedLoss);
        int period=(std::min)(4,1+static_cast<int>(elapsedGameTime(s)/s.quarterSeconds+0.00001f));
        if(period!=capPeriod){capPeriod=period;periodStartLoss=currentLoss;}
        int cap=(std::clamp)(maximumPerQuarter,1,50);if(s.playoff)cap=(std::min)(cap,championship(s)?10:20);
        int allowed=(std::min)(lossCeiling(s),periodStartLoss+cap);
        int requested=(std::min)(100,currentLoss+(std::clamp)(extra,0,25));
        // Fold verified external late-score exits into the same history once.
        double retained=(std::max)(0.000001,1-requested/100.0);
        double anchor=-std::log(retained);
        if(requested>committedLoss)anchor+=(std::max)(0.0,hazard-committedHazard);
        hazard=(std::max)(hazard,anchor);
        int loss=static_cast<int>(std::floor(100*(1-std::exp(-hazard))+0.00001));
        r.loss=(std::max)(currentLoss,(std::clamp)(loss,0,allowed));
        // Discard blocked departures; do not store a catch-up batch for Q3/Q4.
        if(loss>r.loss)hazard=-std::log((std::max)(0.000001,1-r.loss/100.0));
        committedHazard=hazard;committedLoss=r.loss;
        r.changed=r.loss!=before;return r;
    }
};
inline const char* reason(const Settings& c,const State& s,const History& h){
    if(h.comeback&&inferiorOpponent(s)<1)return "comeback protection";
    if(competitiveOvertime(s))return "competitive overtime";
    if(h.watchStage>0)return "upset interest is slowing departures";
    if(s.playoff)return "patient postseason crowd";
    if(std::abs(s.homeScore-s.awayScore)<=8)return "competitive game; routine departures only";
    if(profileFor(c,s)==Profile::BadWeather)return "score, time remaining and bad weather";
    if(!s.neutral&&s.homeScore>s.awayScore&&inferiorOpponent(s)<1)return "home lead is keeping more fans around";
    return "score, time remaining and matchup";
}
}
