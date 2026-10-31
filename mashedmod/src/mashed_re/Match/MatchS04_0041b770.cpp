// MATCH-S04: port body of 0x0041b770 and 0x0041c010 = the byte-matched source re/match/s04/0041b770.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is the only body of both RVAs in the targets that compile this TU
// (0x0041b770 Copy41b770 moved out of Util/PromoLoop_sessionB.cpp, 0x0041c010 HudConstTableInitAndSweep
// out of HUD/ScenarioLeaves_sa2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// 0x0041b770 takes its record in ECX (call site 0x0041c062): under C++ the source declares it
// __fastcall (MATCH_ECX), the convention the old Copy41b770 had, so the export stays @Copy41b770@4.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041b770 Copy41b770
#define FUN_0041c010 HudConstTableInitAndSweep
#pragma comment(linker, "/EXPORT:@Copy41b770@4=@Copy41b770@4")
#pragma comment(linker, "/EXPORT:HudConstTableInitAndSweep=_HudConstTableInitAndSweep")
extern "C" {
#include "../../../../re/match/s04/0041b770.c"
}
RH_ScopedInstall(Copy41b770, 0x0041b770);
RH_ScopedInstall(HudConstTableInitAndSweep, 0x0041c010);
