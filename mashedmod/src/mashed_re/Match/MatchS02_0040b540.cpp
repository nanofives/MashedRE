// MATCH-S02: port body of 0x0040b540 = the byte-matched source re/match/s02/0040b540.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Gameplay/ScoreMasks_ah3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b540 PlayerScoreRankOrder
#define FUN_00417740 Float89a870ToInt
// Callee 0x00417740: Float89a870ToInt (Match/MatchGFFU_00417740.cpp, re/match/gffu/00417740.c); the old body called 0x00417740 through its address.
#pragma comment(linker, "/EXPORT:PlayerScoreRankOrder=_PlayerScoreRankOrder")
extern "C" {
#include "../../../../re/match/s02/0040b540.c"
}
RH_ScopedInstall(PlayerScoreRankOrder, 0x0040b540);
