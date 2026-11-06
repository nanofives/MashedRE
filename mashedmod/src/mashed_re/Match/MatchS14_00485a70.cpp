// MATCH-S14: port body of 0x00485a70 = the byte-matched source re/match/s14/00485a70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round45.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00485a70 PoolInsert485a70
#pragma comment(linker, "/EXPORT:PoolInsert485a70=_PoolInsert485a70")
extern "C" {
#include "../../../../re/match/s14/00485a70.c"
}
RH_ScopedInstall(PoolInsert485a70, 0x00485a70);
