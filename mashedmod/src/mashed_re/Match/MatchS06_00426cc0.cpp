// MATCH-S06: port body of 0x00426cc0 = the byte-matched source re/match/s06/00426cc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/PromoLoop_round2.cpp and Race/PathProgStep_naked.inc PP_426cc0).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426cc0 VehicleTable4cPtrGet
#pragma comment(linker, "/EXPORT:VehicleTable4cPtrGet=_VehicleTable4cPtrGet")
extern "C" {
#include "../../../../re/match/s06/00426cc0.c"
}
RH_ScopedInstall(VehicleTable4cPtrGet, 0x00426cc0);
