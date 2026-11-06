// MATCH-S14: port body of 0x004899c0 = the byte-matched source re/match/s14/004899c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004899c0 Fill4899c0
#pragma comment(linker, "/EXPORT:Fill4899c0=_Fill4899c0")
extern "C" {
#include "../../../../re/match/s14/004899c0.c"
}
RH_ScopedInstall(Fill4899c0, 0x004899c0);
