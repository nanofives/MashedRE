// MATCH-S06: port body of 0x00428760 = the byte-matched source re/match/s06/00428760.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round256.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00428610 -> ViewportScaledRectDraw (symmap swap-batch2 sw5)
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00428760 ViewportScaledRectDraw_Arg7Zero
#define FUN_00428610 ViewportScaledRectDraw
#pragma comment(linker, "/EXPORT:ViewportScaledRectDraw_Arg7Zero=_ViewportScaledRectDraw_Arg7Zero")
extern "C" {
#include "../../../../re/match/s06/00428760.c"
}
RH_ScopedInstall(ViewportScaledRectDraw_Arg7Zero, 0x00428760);
