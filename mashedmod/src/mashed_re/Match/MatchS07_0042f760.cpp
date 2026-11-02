// MATCH-S07: port body of 0x0042f760 = the byte-matched source re/match/s07/0042f760.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round31.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042f760 Global67f19cGet
#pragma comment(linker, "/EXPORT:Global67f19cGet=_Global67f19cGet")
extern "C" {
#include "../../../../re/match/s07/0042f760.c"
}
RH_ScopedInstall(Global67f19cGet, 0x0042f760);
