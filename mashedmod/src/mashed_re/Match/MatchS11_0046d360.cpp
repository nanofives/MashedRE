// MATCH-S11: port body of 0x0046d360 = the byte-matched source re/match/s11/0046d360.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round56.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046d360 Idx2Wheel881738Get
#pragma comment(linker, "/EXPORT:Idx2Wheel881738Get=_Idx2Wheel881738Get")
extern "C" {
#include "../../../../re/match/s11/0046d360.c"
}
RH_ScopedInstall(Idx2Wheel881738Get, 0x0046d360);
