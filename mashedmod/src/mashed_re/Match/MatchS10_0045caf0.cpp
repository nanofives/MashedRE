// MATCH-S10: port body of 0x0045caf0 = the byte-matched source re/match/s10/0045caf0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round58.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045caf0 EqPredicate7f1a18
#pragma comment(linker, "/EXPORT:EqPredicate7f1a18=_EqPredicate7f1a18")
extern "C" {
#include "../../../../re/match/s10/0045caf0.c"
}
RH_ScopedInstall(EqPredicate7f1a18, 0x0045caf0);
