// MATCH-S14: port body of 0x0048a630 = the byte-matched source re/match/s14/0048a630.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0048a630 Aabb48a630
#pragma comment(linker, "/EXPORT:Aabb48a630=_Aabb48a630")
extern "C" {
#include "../../../../re/match/s14/0048a630.c"
}
RH_ScopedInstall(Aabb48a630, 0x0048a630);
