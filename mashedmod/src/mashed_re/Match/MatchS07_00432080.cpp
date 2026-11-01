// MATCH-S07: port body of 0x00432080 = the byte-matched source re/match/s07/00432080.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/ReplayCluster.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00432080 RaceEndCheckFinish
#define FUN_0040e470 MatchS07_00432080_Call0040e470
#define FUN_0042d3a0 RaceTransitionBufZero
// Callees of the matched source: the old Vehicle/ReplayCluster.cpp body called FUN_0040e470 (kFn_FUN_0040e470,
// __cdecl int(int)) and FUN_0042d3a0 (kFn_FUN_0042d3a0, __cdecl void()) through their original addresses. Both now
// bind to their single port bodies: CarSlotStateGet (Ai/CarSlotState.cpp, since D4-DC-INC-ai) and
// RaceTransitionBufZero (Match/MatchS07_0042d3a0.cpp, this slice).
extern "C" unsigned int __cdecl CarSlotStateGet(int param_1);
extern "C" int MatchS07_00432080_Call0040e470(int p0) {
    return static_cast<int>(CarSlotStateGet(p0));
}
#pragma comment(linker, "/EXPORT:RaceEndCheckFinish=_RaceEndCheckFinish")
extern "C" {
#include "../../../../re/match/s07/00432080.c"
}
RH_ScopedInstall(RaceEndCheckFinish, 0x00432080);
