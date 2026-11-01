// MATCH-S07: port body of 0x0042b770 = the byte-matched source re/match/s07/0042b770.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuButtonDetect.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042b770 MenuButtonDetectE
#define FUN_0040e470 MatchS07_0042b770_Call0040e470
// Callee of the matched source: the old Frontend/MenuButtonDetect.cpp body called FUN_0040e470 through its original
// address (CallSlotState, a __cdecl function-pointer cast to 0x0040e470, in both targets), so the forwarder makes that same call.
extern "C" unsigned int MatchS07_0042b770_Call0040e470(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int)>(0x0040e470u)(p0);
}
#pragma comment(linker, "/EXPORT:MenuButtonDetectE=_MenuButtonDetectE")
extern "C" {
#include "../../../../re/match/s07/0042b770.c"
}
RH_ScopedInstall(MenuButtonDetectE, 0x0042b770);
