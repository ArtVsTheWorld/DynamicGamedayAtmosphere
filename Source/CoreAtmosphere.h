#pragma once
#include "SustainedCrowd.h"
#include "AudioLevels.h"

namespace attendance::core {
inline constexpr float NormalMaximumDb=4, EruptionMaximumDb=6;
inline constexpr float NormalMaximumPercent=58.48932f,EruptionMaximumPercent=99.52623f;
inline constexpr double EruptionSeconds=45,RoarHoldSeconds=35,OpeningReleaseSeconds=20;
struct Settings {bool enabled=true,opening=true,eruptions=true,overtime=true;float maximumPercent=NormalMaximumPercent,eruptionPercent=EruptionMaximumPercent;};
inline float normalLimit(const Settings& c){return c.enabled&&std::isfinite(c.maximumPercent)?c.maximumPercent>=NormalMaximumPercent?NormalMaximumDb:volume::boostDb(c.maximumPercent):0;}
inline float eruptionLimit(const Settings& c){return (std::max)(normalLimit(c),std::isfinite(c.eruptionPercent)?c.eruptionPercent>=EruptionMaximumPercent?EruptionMaximumDb:volume::boostDb(c.eruptionPercent):0);}
inline float callbackLimit(const Settings& c,float observed,uint64_t now,uint64_t until){
    float normal=normalLimit(c);if(normal<=0||!c.eruptions||now>=until||!std::isfinite(observed))return normal;
    return (std::clamp)(observed,normal,eruptionLimit(c));
}
inline bool decisive(energy::Event e){return e==energy::Event::LastSecondLead||e==energy::Event::OvertimeWin||e==energy::Event::GameWinningScore;}
struct Model {
    energy::OpeningProfile openingProfile;
    float parent=0,beds=0,limit=4,openingWeight=0,intensity=0,carryDb=0;
    double eruptionAt=-1,postgameAt=-1,openingReleaseAt=-1;energy::Event event=energy::Event::None;
    uint64_t sequence=0,celebration=0;bool erupting=false,sawLive=false,postgameSeen=false,postgameHeld=false,postgameMenu=false;int winner=0;
    void reset(){*this=Model{};}
    float eruptionWeight(double now)const{
        double age=now-eruptionAt;if(postgameMenu||eruptionAt<0||age<0)return 0;
        if(postgameHeld)return static_cast<float>((std::min)(age,1.0));
        if(age>=EruptionSeconds)return 0;
        return age<1?static_cast<float>(age):age<=20?1.0f:static_cast<float>((EruptionSeconds-age)/25);
    }
    bool finishPostgame(){
        if(!postgameSeen||postgameMenu)return false;
        bool held=postgameHeld;postgameMenu=true;postgameHeld=erupting=false;parent=beds=intensity=0;return held;
    }
    uint64_t eruptionLease(uint64_t now)const{
        return !erupting?0:postgameHeld?now+750:static_cast<uint64_t>((eruptionAt+EruptionSeconds)*1000);
    }
    bool react(const Settings& c,const energy::Outcome& result,double now,const State& s){
        if(result.accepted&&result.visitorScore&&!s.neutral){eruptionAt=-1;postgameHeld=false;event=energy::Event::None;return false;}
        if(postgameMenu)return false;
        if(!c.enabled||!c.eruptions||normalLimit(c)<=0||!result.accepted||!decisive(result.event)||!std::isfinite(now))return false;
        // The verified tracker supplies only home scores, or either side at a neutral venue.
        if(result.side==1&&!s.neutral)return false;
        if(result.event==energy::Event::LastSecondLead&&(s.quarter!=4||s.seconds<0||s.seconds>90))return false;
        bool same=result.sequence&&result.sequence==sequence;
        if(same&&energy::priority(result.event)<=energy::priority(event))return false;
        // A postgame confirmation upgrades the existing score; never restart its roar.
        if(same&&eruptionAt>=0){event=result.event;return false;}
        if(eruptionAt>=0&&now-eruptionAt<8&&energy::priority(result.event)<=energy::priority(event))return false;
        eruptionAt=now;event=result.event;sequence=result.sequence;winner=result.side;++celebration;return true;
    }
    void update(const Settings& c,const energy::Play& p,double now,float fraction,float permanentFraction){
        if(!valid(p.game)||!std::isfinite(now)||!std::isfinite(fraction)||!std::isfinite(permanentFraction))return;
        const State& s=p.game;float normal=normalLimit(c);limit=normal;
        bool ranked=top25(s.rank0)&&top25(s.rank1),big=s.rivalry||ranked||s.playoff||championship(s);
        float previousOpening=openingWeight,previousIntensity=intensity;
        openingWeight=openingProfile.observe(s,p.offense,p.down,c.enabled&&c.opening&&normal>0,big);
        if(previousOpening>0&&openingProfile.fade>=0&&openingReleaseAt<0)openingReleaseAt=now;
        int margin=std::abs(s.homeScore-s.awayScore);
        if(s.phase!=6&&s.phase!=7)sawLive=true;
        if(s.phase==7&&!postgameSeen){
            postgameSeen=true;State story=energyPerspective(s);
            bool carrying=eruptionAt>=0&&now>=eruptionAt&&now-eruptionAt<EruptionSeconds;
            bool upset=story.homeScore>story.awayScore&&margin<=8&&upsetDiscrepancy(s)>=0.45f;
            if(sawLive&&c.enabled&&c.eruptions&&normal>0&&upset&&!carrying){
                eruptionAt=now;event=energy::Event::UpsetWin;winner=s.neutral&&s.awayScore>s.homeScore?1:0;++celebration;
            }
            bool winnerMatches=winner==0?s.homeScore>s.awayScore:s.neutral&&s.awayScore>s.homeScore;
            postgameHeld=c.enabled&&c.eruptions&&normal>0&&winnerMatches&&eruptionAt>=0&&now>=eruptionAt&&now-eruptionAt<EruptionSeconds;
        }
        float close=margin<=8?1.0f:margin<=16?0.7f:margin<=24?0.3f:0;
        float period=s.quarter==2||s.quarter==3?0.85f:1;
        float crowd=std::sqrt((std::clamp)(fraction,0.0f,1.0f));
        float identity=championship(s)?normal*(2.0f/3):s.playoff||(s.rivalry&&ranked)?normal*0.5f:big?normal/3:0;
        float steady=identity*close*period*crowd;
        beds=big?normal*close*period*crowd:0;
        if(openingWeight>0){
            float hype=openingWeight*close*std::sqrt((std::clamp)(permanentFraction,0.0f,1.0f));
            steady=(std::max)(steady,normal*0.5f*hype);beds=(std::max)(beds,normal*hype);
        }
        if(c.overtime&&overtime(s)&&s.phase!=7){
            steady=(std::max)(steady,normal*0.5f*crowd);beds=(std::max)(beds,normal*crowd);
        }
        // Preserve the outgoing ambience when an OT winner jumps straight to postgame.
        if(s.phase==7){
            if(postgameAt<0){postgameAt=now;carryDb=parent;}
            double age=now-postgameAt;float carry=age<0||age>=10?0:age<=4?1.0f:static_cast<float>((10-age)/6);
            steady=postgameMenu?0:(std::min)(normal,carryDb)*carry;beds=steady;
        }
        float weight=c.enabled&&c.eruptions?eruptionWeight(now):0;
        erupting=!postgameMenu&&eruptionAt>=0&&now>=eruptionAt&&(postgameHeld||now-eruptionAt<EruptionSeconds)&&c.enabled&&c.eruptions;
        if(normal<=0||!c.enabled){parent=beds=intensity=openingWeight=0;erupting=false;return;}
        limit=normal+(eruptionLimit(c)-normal)*weight;
        parent=(std::clamp)((std::max)(steady,steady+(eruptionLimit(c)-steady)*weight*crowd),0.0f,limit);
        beds=(std::clamp)(beds,0.0f,limit);
        float openingTail=openingReleaseAt>=0?1-(std::clamp)(static_cast<float>((now-openingReleaseAt)/OpeningReleaseSeconds),0.0f,1.0f):0;
        intensity=erupting?1000*weight*crowd:
            c.overtime&&overtime(s)&&s.phase!=7?1000*crowd:
            openingWeight>0&&openingProfile.fade<0?1000*std::sqrt((std::clamp)(permanentFraction,0.0f,1.0f)):
            1000*openingTail*std::sqrt((std::clamp)(permanentFraction,0.0f,1.0f));
        if(erupting&&now-eruptionAt<1)intensity=(std::max)(intensity,previousIntensity);
    }
    sustained::Intent intent(const Settings& c,const energy::Play& p,uintptr_t match,unsigned epoch,uint64_t now,bool allowed)const{
        sustained::Intent out;out.match=match;out.epoch=epoch;out.updated=now;out.offense=p.offense;
        bool phase=p.game.phase==1||p.game.phase==3||p.game.phase==4||p.game.phase==5||p.game.phase==6||p.game.phase==7;
        if(!allowed||!c.enabled||normalLimit(c)<=0||!phase||postgameMenu)return out;
        out.active=true;out.intensity=intensity;
        if(erupting&&c.eruptions){
            out.celebration=celebration;out.celebrationUntil=static_cast<uint64_t>((eruptionAt+6)*1000);
            if(postgameHeld||now/1000.0-eruptionAt<RoarHoldSeconds){
                out.reason=sustained::Reason::Eruption;out.priority=6;out.continuous=true;out.size=3;out.releaseMs=10000;
                return out;
            }
        }
        if(c.overtime&&overtime(p.game)&&p.game.phase!=7){
            out.reason=sustained::Reason::Overtime;out.priority=4;out.continuous=true;out.size=3;return out;
        }
        if(c.opening&&!erupting&&openingWeight>0.2f&&openingProfile.fade<0&&p.game.phase!=7){
            out.reason=sustained::Reason::Opening;out.priority=3;out.continuous=true;out.size=3;out.releaseMs=20000;
        }
        return out;
    }
};
}
