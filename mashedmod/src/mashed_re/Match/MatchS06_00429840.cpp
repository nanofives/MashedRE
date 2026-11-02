// MATCH-S06: port body of 0x00429840 = the byte-matched source re/match/s06/00429840.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round9.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429840 RaceStateLatchSet
#pragma comment(linker, "/EXPORT:RaceStateLatchSet=_RaceStateLatchSet")
extern "C" {
#include "../../../../re/match/s06/00429840.c"
}
RH_ScopedInstall(RaceStateLatchSet, 0x00429840);
