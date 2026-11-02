// MATCH-S11: port body of 0x0046c570 = the byte-matched source re/match/s11/0046c570.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/MiscDamping.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046c570 VehicleDampVec3
#pragma comment(linker, "/EXPORT:VehicleDampVec3=_VehicleDampVec3")
extern "C" {
#include "../../../../re/match/s11/0046c570.c"
}
RH_ScopedInstall(VehicleDampVec3, 0x0046c570);
