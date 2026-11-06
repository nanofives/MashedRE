// MATCH-S19: port body of 0x004c2f00 = the byte-matched source re/match/s19/004c2f00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/VideoConfig.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c2f00 RwEngineGetCurrentMode
#define FUN_004c2c90 MatchS19_004c2f00_Call004c2c90
#pragma comment(linker, "/EXPORT:RwEngineGetCurrentMode=_RwEngineGetCurrentMode")
extern "C" {
#include "../../../../re/match/s19/004c2f00.c"
}
// Raw-address forwarder: the old body called 0x004c2c90 through its raw address (no port body on that path).
extern "C" int MatchS19_004c2f00_Call004c2c90(void *p0, int p1, void *p2, int p3, int p4) {
    return reinterpret_cast<int(__cdecl*)(void *, int, void *, int, int)>(0x004c2c90u)(p0, p1, p2, p3, p4);
}
RH_ScopedInstall(RwEngineGetCurrentMode, 0x004c2f00);
