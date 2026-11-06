// MATCH-S14: port body of 0x00485bd0 = the byte-matched source re/match/s14/00485bd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round48.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00485bd0 StackPop485bd0
#pragma comment(linker, "/EXPORT:StackPop485bd0=_StackPop485bd0")
extern "C" {
#include "../../../../re/match/s14/00485bd0.c"
}
RH_ScopedInstall(StackPop485bd0, 0x00485bd0);
