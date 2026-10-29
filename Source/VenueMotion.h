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
}
