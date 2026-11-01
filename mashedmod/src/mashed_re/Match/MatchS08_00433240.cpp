// MATCH-S08: port body of 0x00433240 = the byte-matched source re/match/s08/00433240.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/RaceArmReset.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00433240 RaceArmReset
#define FUN_00402f80 CupFloatInit
#define FUN_0042d3a0 RaceTransitionBufZero
#pragma comment(linker, "/EXPORT:RaceArmReset=_RaceArmReset")
extern "C" {
#include "../../../../re/match/s08/00433240.c"
}
RH_ScopedInstall(RaceArmReset, 0x00433240);
