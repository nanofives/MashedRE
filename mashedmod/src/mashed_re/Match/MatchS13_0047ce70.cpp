// MATCH-S13: port body of 0x0047ce70 = the byte-matched source re/match/s13/0047ce70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round33.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047ce70 Global6c6eb0Get
#pragma comment(linker, "/EXPORT:Global6c6eb0Get=_Global6c6eb0Get")
extern "C" {
#include "../../../../re/match/s13/0047ce70.c"
}
RH_ScopedInstall(Global6c6eb0Get, 0x0047ce70);
