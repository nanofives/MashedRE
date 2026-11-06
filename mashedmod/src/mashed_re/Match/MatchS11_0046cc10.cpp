// MATCH-S11: port body of 0x0046cc10 = the byte-matched source re/match/s11/0046cc10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/VehicleFloatFieldAsInt.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046cc10 VehicleFloatFieldAsInt
#pragma comment(linker, "/EXPORT:VehicleFloatFieldAsInt=_VehicleFloatFieldAsInt")
extern "C" {
#include "../../../../re/match/s11/0046cc10.c"
}
RH_ScopedInstall(VehicleFloatFieldAsInt, 0x0046cc10);
