// MATCH-S11: port body of 0x004671c0 = the byte-matched source re/match/s11/004671c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/BatchAB_s1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004671c0 GetOverlayCamera
#pragma comment(linker, "/EXPORT:GetOverlayCamera=_GetOverlayCamera")
extern "C" {
#include "../../../../re/match/s11/004671c0.c"
}
RH_ScopedInstall(GetOverlayCamera, 0x004671c0);
