// MATCH-S11: port body of 0x0046dd90 = the byte-matched source re/match/s11/0046dd90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046dd90 VehField8816f4Set
#pragma comment(linker, "/EXPORT:VehField8816f4Set=_VehField8816f4Set")
extern "C" {
#include "../../../../re/match/s11/0046dd90.c"
}
RH_ScopedInstall(VehField8816f4Set, 0x0046dd90);
