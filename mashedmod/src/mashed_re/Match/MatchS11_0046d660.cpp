// MATCH-S11: port body of 0x0046d660 = the byte-matched source re/match/s11/0046d660.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round54.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046d660 VehTbl881f50Get3
#pragma comment(linker, "/EXPORT:VehTbl881f50Get3=_VehTbl881f50Get3")
extern "C" {
#include "../../../../re/match/s11/0046d660.c"
}
RH_ScopedInstall(VehTbl881f50Get3, 0x0046d660);
