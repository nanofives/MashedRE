// MATCH-S13: port body of 0x0047d150 = the byte-matched source re/match/s13/0047d150.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/SlotObjectAccessors.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047d150 SlotObjectField8
#define FUN_0057c210 MatchS13_0047d150_Call0057c210
#pragma comment(linker, "/EXPORT:SlotObjectField8=_SlotObjectField8")
extern "C" {
#include "../../../../re/match/s13/0047d150.c"
// Callee of the matched source: the old Render/SlotObjectAccessors.cpp body called 0x0057c210 through its original
// address (s_FUN_0057c210, a __cdecl function-pointer cast), so the forwarder makes that same call (as MatchS05_00421930).
int MatchS13_0047d150_Call0057c210(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x0057c210u)(p0);
}
}
RH_ScopedInstall(SlotObjectField8, 0x0047d150);
