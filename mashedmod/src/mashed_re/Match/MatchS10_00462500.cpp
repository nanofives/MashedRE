// MATCH-S10: port body of 0x00462500 = the byte-matched source re/match/s10/00462500.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round29.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00462500 Set603868_1
#pragma comment(linker, "/EXPORT:Set603868_1=_Set603868_1")
extern "C" {
#include "../../../../re/match/s10/00462500.c"
}
RH_ScopedInstall(Set603868_1, 0x00462500);
