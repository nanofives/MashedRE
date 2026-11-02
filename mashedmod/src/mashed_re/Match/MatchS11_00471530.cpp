// MATCH-S11: port body of 0x00471530 = the byte-matched source re/match/s11/00471530.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00471530 ClearTable471530
#pragma comment(linker, "/EXPORT:ClearTable471530=_ClearTable471530")
extern "C" {
#include "../../../../re/match/s11/00471530.c"
}
RH_ScopedInstall(ClearTable471530, 0x00471530);
