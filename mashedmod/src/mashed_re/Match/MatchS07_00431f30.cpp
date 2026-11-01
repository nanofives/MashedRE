// MATCH-S07: port body of 0x00431f30 = the byte-matched source re/match/s07/00431f30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00431f30 FrontendPageIdDispatch
// Callee of the matched source: FUN_00431d90 binds to its port body FrontendPanelFlagAdvance
// (Match/MatchS07_00431d90.cpp, both targets). The old Frontend/MenuLeaves_af6.cpp body called it through its
// original address (s_PanelFlagAdvance_af6); the port body is the matched source of that same RVA.
#define FUN_00431d90 FrontendPanelFlagAdvance
#pragma comment(linker, "/EXPORT:FrontendPageIdDispatch=_FrontendPageIdDispatch")
extern "C" {
#include "../../../../re/match/s07/00431f30.c"
}
RH_ScopedInstall(FrontendPageIdDispatch, 0x00431f30);
