// MATCH-S11: port body of 0x0046dd80 = the byte-matched source re/match/s11/0046dd80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round35.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046dd80 Float61313cGet
#pragma comment(linker, "/EXPORT:Float61313cGet=_Float61313cGet")
extern "C" {
#include "../../../../re/match/s11/0046dd80.c"
}
RH_ScopedInstall(Float61313cGet, 0x0046dd80);
