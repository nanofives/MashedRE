// MATCH-S19: port body of 0x004c2e70 = the byte-matched source re/match/s19/004c2e70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RwPluginHelpers_o3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c2e70 RwEngineSetSubSystem
#define FUN_004c2c90 MatchS19_004c2e70_Call004c2c90
#pragma comment(linker, "/EXPORT:RwEngineSetSubSystem=_RwEngineSetSubSystem")
extern "C" {
#include "../../../../re/match/s19/004c2e70.c"
}
// Raw-address forwarder: the old body called 0x004c2c90 through its raw address (no port body on that path).
extern "C" uint MatchS19_004c2e70_Call004c2c90(int p0, undefined4 p1, undefined4 *p2, undefined4 p3, int p4) {
    return reinterpret_cast<uint(__cdecl*)(int, undefined4, undefined4 *, undefined4, int)>(0x004c2c90u)(p0, p1, p2, p3, p4);
}
RH_ScopedInstall(RwEngineSetSubSystem, 0x004c2e70);
