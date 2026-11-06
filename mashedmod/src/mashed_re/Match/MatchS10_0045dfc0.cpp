// MATCH-S10: port body of 0x0045dfc0 = the byte-matched source re/match/s10/0045dfc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Lane2/L2_0045dfc0.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045dfc0 L2_FUN_0045dfc0
#define FUN_0045df70 FloatStep45df70
#pragma comment(linker, "/EXPORT:L2_FUN_0045dfc0=_L2_FUN_0045dfc0")
extern "C" {
#include "../../../../re/match/s10/0045dfc0.c"
}
RH_ScopedInstall(L2_FUN_0045dfc0, 0x0045dfc0);
