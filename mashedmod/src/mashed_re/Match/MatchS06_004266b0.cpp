// MATCH-S06: port body of 0x004266b0 = the byte-matched source re/match/s06/004266b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/FrameWorldPasses.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004266b0 WorldRenderDispatch_End
#define FUN_004e4350 Clear4e4350
#pragma comment(linker, "/EXPORT:WorldRenderDispatch_End=_WorldRenderDispatch_End")
extern "C" {
#include "../../../../re/match/s06/004266b0.c"
}
RH_ScopedInstall(WorldRenderDispatch_End, 0x004266b0);
