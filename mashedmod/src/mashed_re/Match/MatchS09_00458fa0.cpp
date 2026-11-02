// MATCH-S09: port body of 0x00458fa0 = the byte-matched source re/match/s09/00458fa0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Gameplay/PickupPoolSpawn.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00458fa0 TtlSet68b1b0
#pragma comment(linker, "/EXPORT:TtlSet68b1b0=_TtlSet68b1b0")
extern "C" {
#include "../../../../re/match/s09/00458fa0.c"
}
RH_ScopedInstall(TtlSet68b1b0, 0x00458fa0);
