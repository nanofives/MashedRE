// MATCH-S16: port body of 0x00497000 = the byte-matched source re/match/s16/00497000.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00497000 Proj497000
#pragma comment(linker, "/EXPORT:Proj497000=_Proj497000")
extern "C" {
#include "../../../../re/match/s16/00497000.c"
}
RH_ScopedInstall(Proj497000, 0x00497000);
