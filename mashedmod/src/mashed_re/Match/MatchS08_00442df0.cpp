// MATCH-S08: port body of 0x00442df0 = the byte-matched source re/match/s08/00442df0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/PromoLoop_round2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00442df0 RaceFloat898980Get
#pragma comment(linker, "/EXPORT:RaceFloat898980Get=_RaceFloat898980Get")
extern "C" {
#include "../../../../re/match/s08/00442df0.c"
}
RH_ScopedInstall(RaceFloat898980Get, 0x00442df0);
