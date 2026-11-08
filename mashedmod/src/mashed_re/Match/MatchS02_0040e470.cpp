// PORT-STATE-02 (2026-11-08, L-101 seed default ON): port body of 0x0040e470 = the byte-matched source re/match/s02/0040e470.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in both targets (moved out of Ai/CarSlotState.cpp). Compiled here as C++
// with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Its file-backed .data reads come from
// the original in the .asi and from the port-state seed (Compat/PortStateSeed.cpp) in the exe.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e470 CarSlotStateGet
#pragma comment(linker, "/EXPORT:CarSlotStateGet=_CarSlotStateGet")
extern "C" {
#include "../../../../re/match/s02/0040e470.c"
}
RH_ScopedInstall(CarSlotStateGet, 0x0040e470);
