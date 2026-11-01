// MATCH-TOPDOWN-T13: port body of 0x0040e350 = the byte-matched source re/match/s02/0040e350.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// This RVA's only body in both targets (moved out of Ai/PromoLoop_round1.cpp, asi-only). The exe calls it from
// TG_SubMode (Ai/AiTargetClosest.cpp), the call target of TG_414490's two calls at 0x00414493 / 0x0041449d.
// Compiled here as C++ with MATCH_PORT; DAT_0063ba8c binds to its original address (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e350 GetRenderSubMode
#pragma comment(linker, "/EXPORT:GetRenderSubMode=_GetRenderSubMode")
extern "C" {
// 0x0040e350
#include "../../../../re/match/s02/0040e350.c"
}
RH_ScopedInstall(GetRenderSubMode, 0x0040e350);
