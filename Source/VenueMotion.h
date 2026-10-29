#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "PackedHouse.h"
namespace attendance::motion {
inline float venueScale(uint64_t positions){
    if(positions<512||positions>12800000)return 0;
    return packed::interpolate(positions,.35,1.10);
}
inline float occupiedScale(uint64_t positions,float fullness){
    if(positions<512||positions>12800000||!std::isfinite(fullness)||fullness<=0)return 0;
    double occupied=positions*static_cast<double>((std::clamp)(fullness,0.f,1.f));
    double size=(std::clamp)((occupied-packed::SmallPositions)/(packed::LargePositions-packed::SmallPositions),0.,1.);
    return static_cast<float>(.35+.75*size);
}
}
