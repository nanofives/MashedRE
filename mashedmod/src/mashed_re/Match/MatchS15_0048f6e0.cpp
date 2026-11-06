// MATCH-S15: port body of 0x0048f6e0 = the byte-matched source re/match/s15/0048f6e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round53.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0048f6e0 StridedClear770718
#pragma comment(linker, "/EXPORT:StridedClear770718=_StridedClear770718")
extern "C" {
#include "../../../../re/match/s15/0048f6e0.c"
}
RH_ScopedInstall(StridedClear770718, 0x0048f6e0);
