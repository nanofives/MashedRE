// MATCH-S11: port body of 0x0046bd60 = the byte-matched source re/match/s11/0046bd60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round56.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046bd60 Idx2Wheel881744Get
#pragma comment(linker, "/EXPORT:Idx2Wheel881744Get=_Idx2Wheel881744Get")
extern "C" {
#include "../../../../re/match/s11/0046bd60.c"
}
RH_ScopedInstall(Idx2Wheel881744Get, 0x0046bd60);
