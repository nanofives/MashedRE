// MATCH-S06: port body of 0x00428610 = the byte-matched source re/match/s06/00428610.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/ViewportScaledRectDraw.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x0042b8b0 -> ScreenWidthGet (Frontend/MenuButtonDetect.cpp)
//   0x0042b8c0 -> ScreenHeightGet (Frontend/MenuButtonDetect.cpp)
//   0x00450b10 -> HudIm2DQuad (Frontend/DrawQuadPrimitives.cpp)
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00428610 ViewportScaledRectDraw
#define FUN_0042b8b0 ScreenWidthGet
#define FUN_0042b8c0 ScreenHeightGet
#define FUN_00450b10 HudIm2DQuad
#pragma comment(linker, "/EXPORT:ViewportScaledRectDraw=_ViewportScaledRectDraw")
extern "C" {
#include "../../../../re/match/s06/00428610.c"
}
RH_ScopedInstall(ViewportScaledRectDraw, 0x00428610);
