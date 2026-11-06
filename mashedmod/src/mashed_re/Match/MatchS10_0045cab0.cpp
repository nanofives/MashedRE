// MATCH-S10: port body of 0x0045cab0 = the byte-matched source re/match/s10/0045cab0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045cab0 SlotAliveState45cab0
#define FUN_0046cbb0 VehicleCarStateRead
#define FUN_0046c7b0 VehicleSlotGetter
#pragma comment(linker, "/EXPORT:SlotAliveState45cab0=_SlotAliveState45cab0")
extern "C" {
#include "../../../../re/match/s10/0045cab0.c"
}
RH_ScopedInstall(SlotAliveState45cab0, 0x0045cab0);
