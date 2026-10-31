// MATCH-S04: port body of 0x0041e140 = the byte-matched source re/match/s04/0041e140.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round33.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041e140 Global63d7e0Get
#pragma comment(linker, "/EXPORT:Global63d7e0Get=_Global63d7e0Get")
extern "C" {
#include "../../../../re/match/s04/0041e140.c"
}
RH_ScopedInstall(Global63d7e0Get, 0x0041e140);
