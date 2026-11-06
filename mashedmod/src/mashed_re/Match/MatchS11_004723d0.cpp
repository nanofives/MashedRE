// MATCH-S11: port body of 0x004723d0 = the byte-matched source re/match/s11/004723d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round51.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004723d0 Init691500
#pragma comment(linker, "/EXPORT:Init691500=_Init691500")
extern "C" {
#include "../../../../re/match/s11/004723d0.c"
}
RH_ScopedInstall(Init691500, 0x004723d0);
