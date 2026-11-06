// MATCH-S15: port body of 0x00490ff0 = the byte-matched source re/match/s15/00490ff0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RainCameraScale.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00490ff0 RainSetCameraScale
#pragma comment(linker, "/EXPORT:RainSetCameraScale=_RainSetCameraScale")
extern "C" {
#include "../../../../re/match/s15/00490ff0.c"
}
RH_ScopedInstall(RainSetCameraScale, 0x00490ff0);
