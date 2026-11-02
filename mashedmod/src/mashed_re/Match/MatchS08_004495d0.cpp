// MATCH-S08: port body of 0x004495d0 = the byte-matched source re/match/s08/004495d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round75.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004495d0 GlobalFieldGet896278
#pragma comment(linker, "/EXPORT:GlobalFieldGet896278=_GlobalFieldGet896278")
extern "C" {
#include "../../../../re/match/s08/004495d0.c"
}
RH_ScopedInstall(GlobalFieldGet896278, 0x004495d0);
