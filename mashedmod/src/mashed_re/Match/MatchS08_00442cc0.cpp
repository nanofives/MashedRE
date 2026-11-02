// MATCH-S08: port body of 0x00442cc0 = the byte-matched source re/match/s08/00442cc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/AiVehicleFieldGetters.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00442cc0 AiVehicleFloat4Get
#pragma comment(linker, "/EXPORT:AiVehicleFloat4Get=_AiVehicleFloat4Get")
extern "C" {
#include "../../../../re/match/s08/00442cc0.c"
}
RH_ScopedInstall(AiVehicleFloat4Get, 0x00442cc0);
