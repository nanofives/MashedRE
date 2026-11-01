// MATCH-S06: port body of 0x00426de0 = the byte-matched source re/match/s06/00426de0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/FrontendLeaves_ad2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426de0 FrontendFloatGet426de0
#pragma comment(linker, "/EXPORT:FrontendFloatGet426de0=_FrontendFloatGet426de0")
extern "C" {
#include "../../../../re/match/s06/00426de0.c"
}
RH_ScopedInstall(FrontendFloatGet426de0, 0x00426de0);
