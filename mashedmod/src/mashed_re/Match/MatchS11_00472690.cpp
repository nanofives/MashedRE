// MATCH-S11: port body of 0x00472690 = the byte-matched source re/match/s11/00472690.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilRandIntRange_wfb0f.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00472690 RandIntInRange
#define FUN_00534870 MatchS11_00472690_Call00534870
// Raw-address forwarder: the old body called the original PRNG at 0x00534870 through its raw address (no port body).
extern "C" unsigned int MatchS11_00472690_Call00534870(void) {
    return reinterpret_cast<unsigned int(__cdecl*)(void)>(0x00534870u)();
}
#pragma comment(linker, "/EXPORT:RandIntInRange=_RandIntInRange")
extern "C" {
#include "../../../../re/match/s11/00472690.c"
}
RH_ScopedInstall(RandIntInRange, 0x00472690);
