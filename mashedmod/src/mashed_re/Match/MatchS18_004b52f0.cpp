// MATCH-S18 (PORT-TOP-04K): port body of 0x004b52f0 = the byte-matched source re/match/s18/004b52f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU. It replaces the naked hand body
// Thunk4b52f0 of Util/PromoLoop_sessionB.cpp, which loaded [esp+4] +0x18 and jumped to 0x004b52c0.
// The callee 0x004b52c0 is called through its original address, as the hand body did, so the 0x004b52c0 hook
// (Match/MatchS18_004b52c0.cpp) stays toggleable on its own. Export name kept (hooks_registry 'thunk_4b52f0').
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004b52f0 Thunk4b52f0
#define FUN_004b52c0 MatchS18_004b52f0_Call004b52c0
#pragma comment(linker, "/EXPORT:Thunk4b52f0=_Thunk4b52f0")
extern "C" {
#include "../../../../re/match/s18/004b52f0.c"

// Call-through to the original address 0x004b52c0 (not a body).
void MatchS18_004b52f0_Call004b52c0(Obj_4b52c0 *p, unsigned int mask, int set) {
    reinterpret_cast<void(__cdecl *)(Obj_4b52c0 *, unsigned int, int)>(0x004b52c0)(p, mask, set);
}
}
RH_ScopedInstall(Thunk4b52f0, 0x004b52f0);
