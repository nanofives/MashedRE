// MATCH-S13: port body of 0x00482730 = the byte-matched source re/match/s13/00482730.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00482730 Init482730
#pragma comment(linker, "/EXPORT:Init482730=_Init482730")
extern "C" {
#include "../../../../re/match/s13/00482730.c"
}
RH_ScopedInstall(Init482730, 0x00482730);
