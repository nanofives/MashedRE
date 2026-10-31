// MATCH-S04: port body of 0x0041ef60 = the byte-matched source re/match/s04/0041ef60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round65.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041ef60 PlayerFieldSet63dc78
#pragma comment(linker, "/EXPORT:PlayerFieldSet63dc78=_PlayerFieldSet63dc78")
extern "C" {
#include "../../../../re/match/s04/0041ef60.c"
}
RH_ScopedInstall(PlayerFieldSet63dc78, 0x0041ef60);
