#pragma once
#include <cstdint>

namespace attendance {
struct MatchLifecycle {
    uintptr_t match=0;bool prepared=false,played=false,leftWorld=false;
    void reset(){*this=MatchLifecycle{};}
    void leave(){if(match)leftWorld=true;}
    bool needsReset(uintptr_t next,bool pregame)const{return match&&(match!=next||leftWorld||(pregame&&played));}
    void join(uintptr_t next,bool pregame){match=next;prepared=prepared||pregame;leftWorld=false;}
    void live(){played=true;}
    bool keepAtKickoff(uintptr_t next)const{return match==next&&prepared&&!played&&!leftWorld;}
};
}
