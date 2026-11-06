// MATCH-S11: port body of 0x004725f0 = the byte-matched source re/match/s11/004725f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004725f0 InitRange4725f0
#define FUN_0046be10 Write46be10
#pragma comment(linker, "/EXPORT:InitRange4725f0=_InitRange4725f0")
extern "C" {
#include "../../../../re/match/s11/004725f0.c"
}
RH_ScopedInstall(InitRange4725f0, 0x004725f0);
