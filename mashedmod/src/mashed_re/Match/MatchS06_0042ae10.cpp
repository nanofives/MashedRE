// MATCH-S06: port body of 0x0042ae10 = the byte-matched source re/match/s06/0042ae10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuStateMachine.cpp).
// 0x0040e470 binds to the port's CarSlotStateGet (Ai/CarSlotState.cpp); the old body called it at its original address.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042ae10 MenuReadinessCheckA
#define FUN_0040e470 CarSlotStateGet
#pragma comment(linker, "/EXPORT:MenuReadinessCheckA=_MenuReadinessCheckA")
extern "C" {
#include "../../../../re/match/s06/0042ae10.c"
}
RH_ScopedInstall(MenuReadinessCheckA, 0x0042ae10);
