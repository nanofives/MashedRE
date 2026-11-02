// MATCH-S07: port body of 0x0042f520 = the byte-matched source re/match/s07/0042f520.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round11.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042f520 ViewportBlockPtrGet
#pragma comment(linker, "/EXPORT:ViewportBlockPtrGet=_ViewportBlockPtrGet")
extern "C" {
#include "../../../../re/match/s07/0042f520.c"
}
RH_ScopedInstall(ViewportBlockPtrGet, 0x0042f520);
