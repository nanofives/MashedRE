// MATCH-S09: port body of 0x004576b0 = the byte-matched source re/match/s09/004576b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Gameplay/PickupPredicates.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004576b0 AnyActive4576b0
#pragma comment(linker, "/EXPORT:AnyActive4576b0=_AnyActive4576b0")
extern "C" {
#include "../../../../re/match/s09/004576b0.c"
}
RH_ScopedInstall(AnyActive4576b0, 0x004576b0);
