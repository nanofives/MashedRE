// MATCH-S08: port body of 0x0044b000 = the byte-matched source re/match/s08/0044b000.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/SlotObjectAccessors.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0044b000 SlotRecordPositionPtr
#define FUN_0047d150 MatchS08_0044b000_Call0047d150
// Callee of the matched source: the old Render/SlotObjectAccessors.cpp body called 0x0047d150 through its original
// address (s_FUN_0047d150, a __cdecl function-pointer cast), so the forwarder makes that same call.
extern "C" unsigned int MatchS08_0044b000_Call0047d150(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int)>(0x0047d150u)(p0);
}
#pragma comment(linker, "/EXPORT:SlotRecordPositionPtr=_SlotRecordPositionPtr")
extern "C" {
#include "../../../../re/match/s08/0044b000.c"
}
RH_ScopedInstall(SlotRecordPositionPtr, 0x0044b000);
