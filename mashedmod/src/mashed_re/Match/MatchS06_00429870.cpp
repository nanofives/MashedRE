// MATCH-S06: port body of 0x00429870 = the byte-matched source re/match/s06/00429870.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuScoreGetters.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429870 LapTimeALessThanB
#pragma comment(linker, "/EXPORT:LapTimeALessThanB=_LapTimeALessThanB")
extern "C" {
#include "../../../../re/match/s06/00429870.c"
}
RH_ScopedInstall(LapTimeALessThanB, 0x00429870);
