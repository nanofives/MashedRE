// MATCH-S07: port body of 0x0042fe50 = the byte-matched source re/match/s07/0042fe50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuRaceEnd.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042fe50 RaceEndAltFlagIfEndMode
#define FUN_0042f6a0 GetRaceSubMode
#pragma comment(linker, "/EXPORT:RaceEndAltFlagIfEndMode=_RaceEndAltFlagIfEndMode")
extern "C" {
#include "../../../../re/match/s07/0042fe50.c"
}
RH_ScopedInstall(RaceEndAltFlagIfEndMode, 0x0042fe50);
