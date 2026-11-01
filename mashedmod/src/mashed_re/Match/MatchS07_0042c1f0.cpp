// MATCH-S07: port body of 0x0042c1f0 = the byte-matched source re/match/s07/0042c1f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042c1f0 Count42c1f0
#define FUN_00496900 MatchS07_0042c1f0_Call00496900
// Callee of the matched source: the old Util/PromoLoop_sessionB.cpp body called FUN_00496900 through its original
// address (`mov eax,0x496900; call eax`, __cdecl, one int arg), so the forwarder makes that same call.
extern "C" int MatchS07_0042c1f0_Call00496900(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x00496900u)(p0);
}
#pragma comment(linker, "/EXPORT:Count42c1f0=_Count42c1f0")
extern "C" {
#include "../../../../re/match/s07/0042c1f0.c"
}
RH_ScopedInstall(Count42c1f0, 0x0042c1f0);
