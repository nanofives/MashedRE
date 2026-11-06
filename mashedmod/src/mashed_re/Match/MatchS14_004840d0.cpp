// MATCH-S14: port body of 0x004840d0 = the byte-matched source re/match/s14/004840d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round30.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004840d0 JointPtr6ce81cGet
#pragma comment(linker, "/EXPORT:JointPtr6ce81cGet=_JointPtr6ce81cGet")
extern "C" {
#include "../../../../re/match/s14/004840d0.c"
}
RH_ScopedInstall(JointPtr6ce81cGet, 0x004840d0);
