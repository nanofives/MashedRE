// MATCH-S05: port body of 0x004224d0 = the byte-matched source re/match/s05/004224d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuSlotInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004224d0 MenuSlotInit
#define FUN_004b6520 MatchS05_004224d0_Call004b6520
// Callee of the matched source: the old Frontend/MenuSlotInit.cpp body (asi only) called 0x004b6520 through its
// original address (mov eax,0x004b6520 / call eax), so the forwarder makes that same call.
extern "C" void MatchS05_004224d0_Call004b6520(void *p, int n) {
    reinterpret_cast<void (__cdecl*)(void*, int)>(0x004b6520u)(p, n);
}
#pragma comment(linker, "/EXPORT:MenuSlotInit=_MenuSlotInit")
extern "C" {
#include "../../../../re/match/s05/004224d0.c"
}
RH_ScopedInstall(MenuSlotInit, 0x004224d0);
