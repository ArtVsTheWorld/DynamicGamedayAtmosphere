#pragma once
#include <cstdint>
namespace attendance::showcase {
struct Event {const char* name="";int low=0,high=0;explicit operator bool()const{return low>0;}};
// Rivalry.PresentationId -> gameRivalryId, verified against the native rivalry
// records. These identify the event, never the participants' Team IDs. A
// TeamBuilder replacement retaining the rivalry therefore retains its policy.
inline Event event(int rivalry){switch(rivalry){
case 4:return {"Iron Bowl",70,80};
case 14:return {"Army-Navy",90,95};
case 54:return {"Florida-Georgia",70,80};
case 58:return {"Florida State-Miami",70,80};
case 95:return {"Michigan-Ohio State",85,95};
case 116:return {"Notre Dame-USC",70,80};
case 119:return {"Oklahoma-Texas",80,90};
case 139:return {"UCLA-USC",70,80};
default:return {};}}
inline int opening(int rivalry,int week,int year){
    auto policy=event(rivalry);if(!policy)return 0;
    uint32_t n=static_cast<uint32_t>(rivalry)*0x9e3779b9u^static_cast<uint32_t>(week)*0x85ebca6bu^static_cast<uint32_t>(year);
    n^=n>>16;n*=0x7feb352du;n^=n>>15;
    return policy.low+static_cast<int>(n%static_cast<unsigned>(policy.high-policy.low+1));
}
}
