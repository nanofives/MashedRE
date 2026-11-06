// MATCH-S15: port body of 0x004922e0 = the byte-matched source re/match/s15/004922e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/ScenarioWriters_sa2s2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004922e0 CarEventTrigger
#define FUN_0040e470 CarSlotStateGet
#define FUN_0040e350 GetRenderSubMode
#pragma comment(linker, "/EXPORT:CarEventTrigger=_CarEventTrigger")
extern "C" {
#include "../../../../re/match/s15/004922e0.c"
}
RH_ScopedInstall(CarEventTrigger, 0x004922e0);
