// MATCH-S09: port body of 0x0045a0d0 = the byte-matched source re/match/s09/0045a0d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round54.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045a0d0 Table68ba04Get
#pragma comment(linker, "/EXPORT:Table68ba04Get=_Table68ba04Get")
extern "C" {
#include "../../../../re/match/s09/0045a0d0.c"
}
RH_ScopedInstall(Table68ba04Get, 0x0045a0d0);
