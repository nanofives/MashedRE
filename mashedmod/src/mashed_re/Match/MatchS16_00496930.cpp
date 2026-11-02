// MATCH-S16: port body of 0x00496930 = the byte-matched source re/match/s16/00496930.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round30.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00496930 Table773030Get
#pragma comment(linker, "/EXPORT:Table773030Get=_Table773030Get")
extern "C" {
#include "../../../../re/match/s16/00496930.c"
}
RH_ScopedInstall(Table773030Get, 0x00496930);
