// MATCH-S09: port body of 0x00459540 = the byte-matched source re/match/s09/00459540.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round71.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00459540 PoolJReset68ba00
#pragma comment(linker, "/EXPORT:PoolJReset68ba00=_PoolJReset68ba00")
extern "C" {
#include "../../../../re/match/s09/00459540.c"
}
RH_ScopedInstall(PoolJReset68ba00, 0x00459540);
