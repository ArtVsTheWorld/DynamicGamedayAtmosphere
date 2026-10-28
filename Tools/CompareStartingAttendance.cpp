// Reproducible illustrative fixtures, not predictions for the user's current save.
// Compile with the same C++17 toolchain as the runtime; the DGA column uses its real policy.
#include "../Source/StartingAttendance.h"
#include <cstdio>
using namespace attendance::turnout;
float eaPercent(const Seed& s,bool rivalry,bool forceFull){
    if(forceFull)return 100;
    float p=20.f+.6f*(s.schools-s.home.toughest+60-s.home.rank)+.33f*(60-(std::min)(s.schools,s.away.rank))+(rivalry?30.f:0.f);
    if(s.home.homeGames==0&&p<100)p+=.6f*(100-p);
    return (std::clamp)(p,30.f,100.f);
}
void example(const char* name,Seed s,int time=720,int rain=0,bool rivalry=false,bool neutral=false,bool forceFull=false){
    float ea=eaPercent(s,rivalry,forceFull);s.ea=static_cast<int>(s.capacity*ea*.01f);
    Launch l{s.home.id,s.away.id,s.stadium,s.capacity,s.ea,time,rain,rivalry,neutral};auto d=calculate(s,l);
    float revised=100.f*d.attendance/s.capacity;
    char kickoff[32]{};if(d.applied)std::snprintf(kickoff,sizeof(kickoff),"%.1f",revised*d.kickoff*.01f);else std::snprintf(kickoff,sizeof(kickoff),"legacy unchanged");
    std::printf("%s,%.1f,%.1f,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",name,ea,revised,kickoff,d.demand,d.momentum,d.appeal,d.nightBonus,d.weather,d.variation,d.limits,s.home.rank,s.home.toughest,s.home.prestige,s.home.wins,s.home.losses,s.home.homeGames,s.away.rank,s.schools,d.applied?1:0);
}
int main(){
    std::puts("Example,EA settled pct,DGA settled pct,DGA kickoff capacity pct,Demand,Season,Matchup,Night,Weather,Variation,Floor or cap,Home rank,Home atmosphere rank,Prestige,Wins,Losses,Home games,Away rank,League teams,Revised applied");
    Seed s;s.home={1153,50,70,6,3,3,3};s.away={1193,60,80,5,3,3,3};s.stadium=s.homeStadium=553;s.capacity=60000;s.schools=139;s.week=8;s.year=2026;s.regular=true;
    example("Middle-demand ordinary game",s);example("Same matchup - night",s,1200);example("Same matchup - heavy rain",s,720,3);example("Same matchup - rivalry",s,720,0,true);
    auto rankedRival=s;rankedRival.away.rank=10;example("Rivalry with a Top10 visitor",rankedRival,720,0,true);
    example("Same ordinary matchup - EA game-of-week flag",s,720,0,false,false,true);
    auto opener=s;opener.home.wins=opener.home.losses=opener.home.homeGames=0;opener.week=1;example("Middle-demand home opener",opener);
    auto weak=s;weak.home={1164,120,125,3,1,6,4};weak.away={1263,115,120,4,2,5,4};example("Struggling low-demand home team",weak);example("Same struggling matchup - heavy rain",weak,720,3);
    auto elite=s;elite.home={1104,3,1,10,6,0,3};elite.away.rank=55;example("Elite support and winning season",elite);
    auto bowl=elite;bowl.regular=false;example("CFP example - preserved",bowl,1200,3,false,false,true);
    auto neutral=weak;neutral.homeStadium=10;example("Regular-season neutral rivalry",neutral,720,0,true,true);
}
