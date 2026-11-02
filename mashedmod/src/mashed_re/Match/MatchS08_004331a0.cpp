// MATCH-S08: port body of 0x004331a0 = the byte-matched source re/match/s08/004331a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/ScenarioWriters_sa2s2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004331a0 RaceFinalizeOnce
#define FUN_0042d3a0 RaceTransitionBufZero
#define FUN_004248b0 PerCarSnapshotInit
#define FUN_00424920 EndOfRoundAccumulator
#pragma comment(linker, "/EXPORT:RaceFinalizeOnce=_RaceFinalizeOnce")
extern "C" {
#include "../../../../re/match/s08/004331a0.c"
}
RH_ScopedInstall(RaceFinalizeOnce, 0x004331a0);
