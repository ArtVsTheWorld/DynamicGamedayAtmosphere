#pragma once
#include <windows.h>
#include <cstdint>

namespace attendance::visibility {
using PartResolver=unsigned(__fastcall*)(uintptr_t,unsigned);
// The part table can retire before its renderer during scene changes. A failed
// optional inspection must not bypass native handling or fault the whole mod.
inline bool resolvePart(PartResolver resolver,uintptr_t parts,unsigned part,unsigned& bit)noexcept{
    bit=16;
    if(!resolver||parts<0x10000)return false;
    __try{bit=resolver(parts,part);return bit<16;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
