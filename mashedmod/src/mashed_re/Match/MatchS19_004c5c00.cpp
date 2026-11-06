// MATCH-S19: port body of 0x004c5c00 = the byte-matched source re/match/s19/004c5c00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c5c00 Search4c5c00
#pragma comment(linker, "/EXPORT:Search4c5c00=_Search4c5c00")
extern "C" {
#include "../../../../re/match/s19/004c5c00.c"
}
RH_ScopedInstall(Search4c5c00, 0x004c5c00);
