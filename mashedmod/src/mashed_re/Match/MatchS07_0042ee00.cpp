// MATCH-S07: port body of 0x0042ee00 = the byte-matched source re/match/s07/0042ee00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SpriteGate.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042ee00 SpriteSlotGate
#define FUN_0040bb50 MatchS07_0042ee00_Call0040bb50
// Callee of the matched source: the old Frontend/SpriteGate.cpp body tail-jumped to FUN_0040bb50 through its
// original address (s_jmp_0040bb50), so the forwarder makes that same call.
extern "C" void *MatchS07_0042ee00_Call0040bb50(char *key) {
    return reinterpret_cast<void *(__cdecl*)(char *)>(0x0040bb50u)(key);
}
#pragma comment(linker, "/EXPORT:SpriteSlotGate=_SpriteSlotGate")
extern "C" {
#include "../../../../re/match/s07/0042ee00.c"
}
RH_ScopedInstall(SpriteSlotGate, 0x0042ee00);
