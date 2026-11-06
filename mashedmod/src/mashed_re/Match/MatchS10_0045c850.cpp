// MATCH-S10: port body of 0x0045c850 = the byte-matched source re/match/s10/0045c850.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round47.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045c850 Table88f09cClear
#pragma comment(linker, "/EXPORT:Table88f09cClear=_Table88f09cClear")
extern "C" {
#include "../../../../re/match/s10/0045c850.c"
}
RH_ScopedInstall(Table88f09cClear, 0x0045c850);
