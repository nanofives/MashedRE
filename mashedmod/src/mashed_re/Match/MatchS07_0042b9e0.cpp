// MATCH-S07: port body of 0x0042b9e0 = the byte-matched source re/match/s07/0042b9e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, whole symbol).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/MenuButtonDetect.cpp by MATCH-S07-FU, 2026-11-01).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// Default-OFF knob MASHED_CARSLOT_ASSIGN_SLOTSTATE (exe only; PREREG verify/matchs07fu_20261101/PREREG_MATCH-S07-FU.md).
// The original calls FUN_0040e480 (CarSlotStateSet) with (i, 0) for i = 0..3, then (port, 1) once per level match
// in the assignment loop. The old exe copy skipped every one of those calls through a standalone no-op (MenuButtonDetect.cpp
// CallSlotWrite), and the exe calls CarSlotAssign from Team Select (exe_main.cpp, the CarSlotAssign() call). So the
// forwarder below keeps that pre-swap exe behaviour by default: in the exe it calls CarSlotStateSet only when
// MASHED_CARSLOT_ASSIGN_SLOTSTATE is set. Everything else the body computes is the matched text in both targets.
// The .asi forwarder always calls CarSlotStateSet: the old .asi copy called 0x0040e480 by address, which the .asi
// installs to that same CarSlotStateSet, so the .asi is unchanged. Turning the knob on by default is a roadmap
// decision row, not this file's.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdlib>

#define MATCH_PORT 1
#define FUN_0042b9e0 CarSlotAssign
#define FUN_0040e480 MatchS07_0042b9e0_Call0040e480
// Callee of the matched source: 0x0040e480 has one port body in both targets, CarSlotStateSet (Ai/CarSlotState.cpp).
extern "C" void __cdecl CarSlotStateSet(int param_1, std::uint32_t param_2);
extern "C" void MatchS07_0042b9e0_Call0040e480(int p0, int p1) {
#ifdef MASHED_STANDALONE
    static const bool s_slotState = (std::getenv("MASHED_CARSLOT_ASSIGN_SLOTSTATE") != nullptr);
    if (!s_slotState) return;   // pre-swap exe behaviour (default): the call is skipped
#endif
    CarSlotStateSet(p0, static_cast<std::uint32_t>(p1));
}
#pragma comment(linker, "/EXPORT:CarSlotAssign=_CarSlotAssign")
extern "C" {
#include "../../../../re/match/s07/0042b9e0.c"
}
RH_ScopedInstall(CarSlotAssign, 0x0042b9e0);
