// MATCH-S02: port body of 0x0040e470 = the byte-matched source re/match/s02/0040e470.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Ai/CarSlotState.cpp,
// PORT-FE-01 2026-11-08). Compiled here as C++ with MATCH_PORT; the one global, PTR_PTR_005f2770, binds to its
// original address (preamble MGLOBAL), the same address the replaced hand body read. No callees.
// Export name unchanged (CarSlotStateGet), so every by-name caller (AiLeafTargets.cpp, AiTargetClosest.cpp,
// Match/MatchS07_0042af50.cpp) and the install are unchanged. Like the replaced body it carries no null-base guard
// (Ai/CarSlotState.cpp header: the setter keeps its standalone guard, the getter never had one).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e470 CarSlotStateGet
#pragma comment(linker, "/EXPORT:CarSlotStateGet=_CarSlotStateGet")
extern "C" {
#include "../../../../re/match/s02/0040e470.c"
}
RH_ScopedInstall(CarSlotStateGet, 0x0040e470);  // re-enabled 2026-05-24 c3-render-a, moved here PORT-FE-01
