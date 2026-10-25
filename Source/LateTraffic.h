#pragma once
#include "AttendanceContext.h"
namespace attendance::traffic {
inline float window(const State& s){return s.quarterSeconds*.25f;}
inline bool lateHomeGame(const State& s){return valid(s)&&!s.neutral&&s.quarter==4&&s.phase!=7&&s.seconds>=0&&s.seconds<=window(s);}
inline float recentScore(const State& s,float scoredAt){
    float now=elapsedGameTime(s);
    return scoredAt>=0&&now>=scoredAt?std::exp(-(now-scoredAt)/(s.quarterSeconds/8.f)):0;
}
// A recovery outlook for attendance, not a published win-probability estimate.
// Unknown possession/timeouts receive the more patient interpretation.
inline float recovery(const State& s,float recent=0){
    int deficit=s.awayScore-s.homeScore;if(deficit<=0)return 1;
    int scores=(deficit+7)/8;
    int timeouts=s.homeTimeouts>=0&&s.homeTimeouts<=3?s.homeTimeouts:3;
    float opponentPossession=s.offense==1?40.f-10.f*timeouts:0;
    float field=s.offense==0&&s.toGoal>=0&&s.toGoal<=20?25.f:0;
    float needed=scores*65.f+opponentPossession-12.f*timeouts-field;
    float chance=context::smooth((s.seconds-needed+45.f)/90.f);
    float lift=.25f*(std::clamp)(recent,0.f,1.f)*(1-chance)*context::smooth(chance*2);
    return (std::clamp)(chance+lift,0.f,1.f);
}
inline float activation(const State& s){return lateHomeGame(s)?context::smooth((window(s)-s.seconds)/(window(s)*.20f)):0;}
inline float lossTarget(const State& s,float scoredAt=-1){
    int deficit=s.awayScore-s.homeScore;
    if(!lateHomeGame(s)||deficit<=8||(s.playoff&&deficit<17))return 0;
    float hopeless=1-recovery(s,recentScore(s,scoredAt));
    float target=30+45*context::smooth((deficit-8)/27.f);
    if(s.playoff)target=lossCeiling(s)*(target/75.f)*(championship(s)?.35f:.60f);
    return target*std::pow(hopeless,.8f)*activation(s);
}
}
