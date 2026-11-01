// MATCH-S05: port body of 0x00422a80 = the byte-matched source re/match/s05/00422a80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SlotZeroers_s1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00422a80 SlotBlockZero
#define FUN_004b6520 MatchS05_00422a80_Call004b6520
// Callees of the matched source: the old Frontend/SlotZeroers_s1.cpp body called 0x004b6520 through its original address (__cdecl function-pointer cast), so the forwarder makes that same call.
extern "C" void MatchS05_00422a80_Call004b6520(void * p0, unsigned int p1) {
    reinterpret_cast<void (__cdecl*)(void *, unsigned int)>(0x004b6520u)(p0, p1);
}
#pragma comment(linker, "/EXPORT:SlotBlockZero=_SlotBlockZero")
extern "C" {
#include "../../../../re/match/s05/00422a80.c"
}
RH_ScopedInstall(SlotBlockZero, 0x00422a80);
