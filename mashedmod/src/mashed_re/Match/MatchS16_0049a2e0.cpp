// MATCH-S16: port body of 0x0049a2e0 = the byte-matched source re/match/s16/0049a2e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round28.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0049a2e0 Set77396c
#pragma comment(linker, "/EXPORT:Set77396c=_Set77396c")
extern "C" {
#include "../../../../re/match/s16/0049a2e0.c"
}
RH_ScopedInstall(Set77396c, 0x0049a2e0);
