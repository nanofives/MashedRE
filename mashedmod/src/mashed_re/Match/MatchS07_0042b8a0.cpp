// MATCH-S07: port body of 0x0042b8a0 = the byte-matched source re/match/s07/0042b8a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RenderSubmit_o4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042b8a0 RenderHeightSet
#pragma comment(linker, "/EXPORT:RenderHeightSet=_RenderHeightSet")
extern "C" {
#include "../../../../re/match/s07/0042b8a0.c"
}
RH_ScopedInstall(RenderHeightSet, 0x0042b8a0);
