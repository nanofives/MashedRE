// MATCH-S16: port body of 0x0049a740 = the byte-matched source re/match/s16/0049a740.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round28.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0049a740 Set773978
#pragma comment(linker, "/EXPORT:Set773978=_Set773978")
extern "C" {
#include "../../../../re/match/s16/0049a740.c"
}
RH_ScopedInstall(Set773978, 0x0049a740);
