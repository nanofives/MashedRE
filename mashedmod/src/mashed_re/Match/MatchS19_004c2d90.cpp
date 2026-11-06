// MATCH-S19: port body of 0x004c2d90 = the byte-matched source re/match/s19/004c2d90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round249.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c2d90 RwEngineRegisterPlugin
#define FUN_004d7de0 MatchS19_004c2d90_Call004d7de0
#pragma comment(linker, "/EXPORT:RwEngineRegisterPlugin=_RwEngineRegisterPlugin")
extern "C" {
#include "../../../../re/match/s19/004c2d90.c"
}
// Raw-address forwarder: the old body called 0x004d7de0 through its raw address (no port body on that path).
extern "C" int MatchS19_004c2d90_Call004d7de0(undefined4 *p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, int p5) {
    return reinterpret_cast<int(__cdecl*)(undefined4 *, undefined4, undefined4, undefined4, undefined4, int)>(0x004d7de0u)(p0, p1, p2, p3, p4, p5);
}
RH_ScopedInstall(RwEngineRegisterPlugin, 0x004c2d90);
