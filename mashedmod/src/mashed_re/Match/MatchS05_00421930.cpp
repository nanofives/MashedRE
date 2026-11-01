// MATCH-S05: port body of 0x00421930 = the byte-matched source re/match/s05/00421930.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/SlotObjectAccessors.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00421930 BodyGeometryFirstDword
#define FUN_0057c210 MatchS05_00421930_Call0057c210
// Callee of the matched source: the old Render/SlotObjectAccessors.cpp body called 0x0057c210 through its original
// address (s_FUN_0057c210, a __cdecl function-pointer cast), so the forwarder makes that same call.
extern "C" int MatchS05_00421930_Call0057c210(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x0057c210u)(p0);
}
#pragma comment(linker, "/EXPORT:BodyGeometryFirstDword=_BodyGeometryFirstDword")
extern "C" {
#include "../../../../re/match/s05/00421930.c"
}
RH_ScopedInstall(BodyGeometryFirstDword, 0x00421930);
