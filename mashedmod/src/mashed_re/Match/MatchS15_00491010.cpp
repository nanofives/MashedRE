// MATCH-S15: port body of 0x00491010 = the byte-matched source re/match/s15/00491010.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RainLineWidthRange.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00491010 RainLineWidthRangeSet
#pragma comment(linker, "/EXPORT:RainLineWidthRangeSet=_RainLineWidthRangeSet")
extern "C" {
#include "../../../../re/match/s15/00491010.c"
}
RH_ScopedInstall(RainLineWidthRangeSet, 0x00491010);
