// MATCH-S07: port body of 0x0042f0b0 = the byte-matched source re/match/s07/0042f0b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042f0b0 GetFrameCounterPlus73
#pragma comment(linker, "/EXPORT:GetFrameCounterPlus73=_GetFrameCounterPlus73")
extern "C" {
#include "../../../../re/match/s07/0042f0b0.c"
}
RH_ScopedInstall(GetFrameCounterPlus73, 0x0042f0b0);
