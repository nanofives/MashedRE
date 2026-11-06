// MATCH-S11: port body of 0x0046d700 = the byte-matched source re/match/s11/0046d700.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/VehicleState.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046d700 VehicleVec3At9C8Get
#pragma comment(linker, "/EXPORT:VehicleVec3At9C8Get=_VehicleVec3At9C8Get")
extern "C" {
#include "../../../../re/match/s11/0046d700.c"
}
RH_ScopedInstall(VehicleVec3At9C8Get, 0x0046d700);
