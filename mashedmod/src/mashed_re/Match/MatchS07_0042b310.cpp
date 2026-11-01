// MATCH-S07: port body of 0x0042b310 = the byte-matched source re/match/s07/0042b310.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuButtonDetect.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042b310 MenuButtonDetectC
#define FUN_0040e470 MatchS07_0042b310_Call0040e470
// Callee of the matched source: the old Frontend/MenuButtonDetect.cpp body called FUN_0040e470 through its original
// address (CallSlotState, a __cdecl function-pointer cast to 0x0040e470, in both targets). Since orch/integration (D4-DC-INC-ai, 2026-11-01) 0x0040e470 has one port body in both targets,
// CarSlotStateGet (Ai/CarSlotState.cpp, same read of [0x005f2770]+i*4+0x34), so the forwarder calls it.
extern "C" unsigned int __cdecl CarSlotStateGet(int param_1);
extern "C" int MatchS07_0042b310_Call0040e470(int p0) {
    return static_cast<int>(CarSlotStateGet(p0));
}
#pragma comment(linker, "/EXPORT:MenuButtonDetectC=_MenuButtonDetectC")
extern "C" {
#include "../../../../re/match/s07/0042b310.c"
}
RH_ScopedInstall(MenuButtonDetectC, 0x0042b310);
