// MATCH-S07: port body of 0x0042ee40 = the byte-matched source re/match/s07/0042ee40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/FrontendDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042ee40 FrontendModeDispatch
#define FUN_0040bb90 SpriteLookupTableB
#pragma comment(linker, "/EXPORT:FrontendModeDispatch=_FrontendModeDispatch")
extern "C" {
#include "../../../../re/match/s07/0042ee40.c"
}
RH_ScopedInstall(FrontendModeDispatch, 0x0042ee40);
