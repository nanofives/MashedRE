// MATCH-S09: port body of 0x00453f30 = the byte-matched source re/match/s09/00453f30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round74.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00453f30 Fill6870b4
#pragma comment(linker, "/EXPORT:Fill6870b4=_Fill6870b4")
extern "C" {
#include "../../../../re/match/s09/00453f30.c"
}
RH_ScopedInstall(Fill6870b4, 0x00453f30);
