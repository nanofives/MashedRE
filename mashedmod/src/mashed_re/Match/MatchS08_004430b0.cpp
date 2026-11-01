// MATCH-S08: port body of 0x004430b0 = the byte-matched source re/match/s08/004430b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round8.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004430b0 Util897fe0Get
#pragma comment(linker, "/EXPORT:Util897fe0Get=_Util897fe0Get")
extern "C" {
#include "../../../../re/match/s08/004430b0.c"
}
RH_ScopedInstall(Util897fe0Get, 0x004430b0);
