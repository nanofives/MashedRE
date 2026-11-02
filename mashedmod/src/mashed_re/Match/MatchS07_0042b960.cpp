// MATCH-S07: port body of 0x0042b960 = the byte-matched source re/match/s07/0042b960.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/MenuButtonDetect.cpp by MATCH-S07-FU, 2026-11-01).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// What the swap changes: the old exe copy skipped FUN_0040e480 (MenuButtonDetect.cpp CallSlotWrite, a standalone
// no-op); the matched source always calls it, as the original does. No exe caller reaches CarSlotInit1P
// (verify/matchs07fu_20261101/RESULT_MATCH-S07-FU.md), so the default exe runs no new code.
#include "../Core/HookSystem.h"

#include <cstdint>

#define MATCH_PORT 1
#define FUN_0042b960 CarSlotInit1P
#define FUN_0040e480 MatchS07_0042b960_Call0040e480
// Callee of the matched source: 0x0040e480 has one port body in both targets, CarSlotStateSet (Ai/CarSlotState.cpp,
// the write [[0x005f2770] + i*4 + 0x34] = v, guarded for a 0 base in the standalone). The forwarder only adapts the
// prototype (the source declares (int, int)).
extern "C" void __cdecl CarSlotStateSet(int param_1, std::uint32_t param_2);
extern "C" void MatchS07_0042b960_Call0040e480(int p0, int p1) {
    CarSlotStateSet(p0, static_cast<std::uint32_t>(p1));
}
#pragma comment(linker, "/EXPORT:CarSlotInit1P=_CarSlotInit1P")
extern "C" {
#include "../../../../re/match/s07/0042b960.c"
}
RH_ScopedInstall(CarSlotInit1P, 0x0042b960);
