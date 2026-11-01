// MATCH-S07: port body of 0x00431f30 = the byte-matched source re/match/s07/00431f30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00431f30 FrontendPageIdDispatch
#define FUN_00431d90 MatchS07_00431f30_Call00431d90
// Callee of the matched source: the old Frontend/MenuLeaves_af6.cpp body called FUN_00431d90 through its original
// address (s_PanelFlagAdvance_af6, a __cdecl void() function-pointer cast to 0x00431d90), so the forwarder makes
// that same call.
extern "C" void MatchS07_00431f30_Call00431d90(void) {
    reinterpret_cast<void (__cdecl*)(void)>(0x00431d90u)();
}
#pragma comment(linker, "/EXPORT:FrontendPageIdDispatch=_FrontendPageIdDispatch")
extern "C" {
#include "../../../../re/match/s07/00431f30.c"
}
RH_ScopedInstall(FrontendPageIdDispatch, 0x00431f30);
