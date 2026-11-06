// MATCH-S19: port body of 0x004c1c80 = the byte-matched source re/match/s19/004c1c80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/ViewportDimsSet_ag.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c1c80 ViewportDimsSet
#define FUN_004c0e50 MatchS19_004c1c80_Call004c0e50
#pragma comment(linker, "/EXPORT:ViewportDimsSet=_ViewportDimsSet")
extern "C" {
#include "../../../../re/match/s19/004c1c80.c"
}
// Raw-address forwarder: the old body called 0x004c0e50 through its raw address (no port body on that path).
extern "C" int MatchS19_004c1c80_Call004c0e50(int p0) {
    return reinterpret_cast<int(__cdecl*)(int)>(0x004c0e50u)(p0);
}
RH_ScopedInstall(ViewportDimsSet, 0x004c1c80);
