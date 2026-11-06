// MATCH-S13: port body of 0x0047ce40 = the byte-matched source re/match/s13/0047ce40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047ce40 Search47ce40
#pragma comment(linker, "/EXPORT:Search47ce40=_Search47ce40")
extern "C" {
#include "../../../../re/match/s13/0047ce40.c"
}
RH_ScopedInstall(Search47ce40, 0x0047ce40);
