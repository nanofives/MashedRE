// MATCH-S11: port body of 0x00472550 = the byte-matched source re/match/s11/00472550.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round50.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00472550 Table691508Get
#pragma comment(linker, "/EXPORT:Table691508Get=_Table691508Get")
extern "C" {
#include "../../../../re/match/s11/00472550.c"
}
RH_ScopedInstall(Table691508Get, 0x00472550);
