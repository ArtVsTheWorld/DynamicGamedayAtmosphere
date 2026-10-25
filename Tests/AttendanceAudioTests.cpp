#include "../Source/AttendanceAudio.h"
#include "../Source/CrowdAudioModel.h"
#include <cassert>
#include <cstdio>
using namespace attendance;
int main(){
    for(float f=0;f<=1.00001f;f+=.001f){
        float crowd=balance::crowdBoost(f),chant=balance::chantBoost(f);assert(crowd>=0&&crowd<=25&&chant>=0&&chant<=15);
        if(f<=.75f)assert(crowd==0);if(f<=.75f)assert(chant==0);
        auto mix=balance::mixer(crowd,chant);assert(mix.chant+mix.crowd<=volume::boostDb(15)+.001f);
        auto eruption=balance::mixer(70,chant);assert(eruption.crowd<=volume::boostDb(50)+.001f&&eruption.crowd+eruption.chant<=volume::boostDb(15)+.001f);
        audio::Gain g;g.attendance(f);g.advance(6);assert(g.value()<=1);assert(std::fabs(g.value()-(std::min)(1.0f,f/.35f))<.0001f);if(f>=.35f)assert(g.value()==1);
    }
    for(auto sample:{std::pair<float,float>{1.f,25.f},{.95f,20.f},{.9f,15.f},{.85f,10.f},{.8f,5.f},{.75f,0.f}})assert(std::fabs(balance::crowdBoost(sample.first)-sample.second)<.001f);
    assert(std::fabs(balance::chantBoost(1)-15)<.001f&&balance::chantBoost(.75f)==0);
    for(auto sample:{std::pair<float,float>{1.f,15.f},{.95f,12.f},{.9f,9.f},{.85f,6.f},{.8f,3.f},{.75f,0.f}})assert(std::fabs(balance::chantBoost(sample.first)-sample.second)<.001f);
    assert(balance::chantBoost(1,0)==0&&balance::chantBoost(1,7.5f)==7.5f&&balance::chantBoost(1,100)==15);
    audio::Gain cut;cut.attendance(.1f);cut.advance(6);assert(std::fabs(cut.value()-(.1f/.35f))<.001f);cut.attendance(0);cut.advance(6);assert(cut.value()==0);
    std::puts("PASS: linear 75-100 crowd/chant boosts, neutral 35-75 band, below35 common cut to silence and independent chant ceiling during eruptions.");
}
