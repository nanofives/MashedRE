// MATCH-S08: port body of 0x00448700 = the byte-matched source re/match/s08/00448700.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/VehicleSeed.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00448700 VehicleSeedWritePair
#define FUN_004464c0 CameraEntryDispatchAll
#pragma comment(linker, "/EXPORT:VehicleSeedWritePair=_VehicleSeedWritePair")
extern "C" {
#include "../../../../re/match/s08/00448700.c"
}
RH_ScopedInstall(VehicleSeedWritePair, 0x00448700);
