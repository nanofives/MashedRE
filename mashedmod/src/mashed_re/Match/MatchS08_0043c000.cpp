// MATCH-S08: port body of 0x0043c000 = the byte-matched source re/match/s08/0043c000.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

// Raw-address forwarders: callees with no port body, called through the original image as the old body did.
extern "C" void __cdecl MatchS08_0043c000_Call004332a0(void) { reinterpret_cast<void(__cdecl*)(void)>(0x004332a0u)(); }
extern "C" void __cdecl MatchS08_0043c000_Call0042c960(void) { reinterpret_cast<void(__cdecl*)(void)>(0x0042c960u)(); }

#define MATCH_PORT 1
#define FUN_0043c000 TimerSlotTickDispatcher
#define FUN_004332a0 MatchS08_0043c000_Call004332a0
#define FUN_0042f7b0 FrontendCursorUpdate
#define FUN_0042c960 MatchS08_0043c000_Call0042c960
#define FUN_0042fa00 MenuTeamSelectTick
#pragma comment(linker, "/EXPORT:TimerSlotTickDispatcher=_TimerSlotTickDispatcher")
extern "C" {
#include "../../../../re/match/s08/0043c000.c"
}
RH_ScopedInstall(TimerSlotTickDispatcher, 0x0043c000);
