// MATCH-S02: port body of 0x0040b620 = the byte-matched source re/match/s02/0040b620.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuScoreGetters.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b620 SlotSortByModeScore
#pragma comment(linker, "/EXPORT:SlotSortByModeScore=_SlotSortByModeScore")
extern "C" {
#include "../../../../re/match/s02/0040b620.c"
}
RH_ScopedInstall(SlotSortByModeScore, 0x0040b620);
