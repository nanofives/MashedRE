// MATCH-S06: port body of 0x00426670 = the byte-matched source re/match/s06/00426670.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/FrameWorldPasses.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426670 WorldRenderDispatch_Begin
#define FUN_004e4320 RwPluginLinkSet
#pragma comment(linker, "/EXPORT:WorldRenderDispatch_Begin=_WorldRenderDispatch_Begin")
extern "C" {
#include "../../../../re/match/s06/00426670.c"
}
RH_ScopedInstall(WorldRenderDispatch_Begin, 0x00426670);
