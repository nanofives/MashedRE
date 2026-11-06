// MATCH-S12: port body of 0x004759b0 = the byte-matched source re/match/s12/004759b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_standins_r1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004759b0 Arrays691600Alloc
#define FUN_00474db0 RwMallocArrayZeroFill
#pragma comment(linker, "/EXPORT:Arrays691600Alloc=_Arrays691600Alloc")
extern "C" {
#include "../../../../re/match/s12/004759b0.c"
}
RH_ScopedInstall(Arrays691600Alloc, 0x004759b0);
