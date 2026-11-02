// MATCH-S15: port body of 0x00492440 = the byte-matched source re/match/s15/00492440.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RenderLeaves_ae1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00492440 RenderStatsAccumulate
#pragma comment(linker, "/EXPORT:RenderStatsAccumulate=_RenderStatsAccumulate")
extern "C" {
#include "../../../../re/match/s15/00492440.c"
}
RH_ScopedInstall(RenderStatsAccumulate, 0x00492440);
