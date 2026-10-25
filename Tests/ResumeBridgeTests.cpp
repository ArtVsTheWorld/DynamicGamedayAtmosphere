#include <cassert>
#include <fstream>
#include <iostream>
#include "../Source/ResumeBridge.h"
using namespace attendance::resume;
int main(){
    const std::string text=R"({"product":"midgamesave","source":"dll","pid":9716,"game":{"active":true,"number":3,"ended":false,"live":{"quarter":4,"clock":47.53,"period":300,"home_score":31,"away_score":33}},"arm":{"state":"applied","applied_game":3,"situation":{"period":900}},"ignored":[null,true,{"text":"a\\b\"c\u1234"}]})";
    auto s=parse(text,9716,1000);assert(s.valid&&s.game==3&&s.applied&&s.live.quarterSeconds==300);
    assert(!parse(text,1,1000).valid);assert(!parse(text.substr(0,text.size()-1),9716,1000).valid);
    auto live=s.live;live.phase=1;assert(s.matches(live,2000));assert(!s.matches(live,11001));
    Tracker tracker;assert(tracker.pending(s,live,1000)&&tracker.pending(s,live,1000));
    assert(!tracker.game&&!tracker.applied); // Replay ownership checks must not consume the scenario signal.
    assert(tracker.observe(s,live,1000));assert(!tracker.pending(s,live,1001)&&!tracker.observe(s,live,1001));
    auto stale=s;stale.applied=false;stale.game=4;stale.live.homeScore=0;assert(!tracker.observe(stale,live,1002));
    stale.live=live;assert(tracker.observe(stale,live,1003));assert(!tracker.observe(stale,live,1004));
    stale.applied=true;assert(tracker.observe(stale,live,1005));assert(!tracker.observe(stale,live,1006));
    // A previous game's applied marker must not count as this game's resume.
    auto foreign=text;foreign.replace(foreign.find("\"applied_game\":3"),16,"\"applied_game\":2");
    auto wrong=parse(foreign,9716,1000);assert(wrong.valid&&!wrong.applied);
    Json json;
    for(const char* invalid:{"{\"a\":1,\"a\":2}","{\"a\":01}","{\"a\":1.}","{\"a\":true,}","{\"a\":\"\\x\"}","{\"a\":1e9999}","[] trailing"})assert(!Reader(std::string(invalid)).parse(json));
    assert(!Reader(std::string(65537,' ')).parse(json));
    std::cout<<"PASS: bounded JSON parsing, PID/freshness/score validation, scenario generations and one-shot resume detection\n";
}
