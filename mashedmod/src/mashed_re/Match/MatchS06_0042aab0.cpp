// MATCH-S06: port body of 0x0042aab0 = the byte-matched source re/match/s06/0042aab0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilLeaves_ac.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042aab0 UtilInit898a90
#pragma comment(linker, "/EXPORT:UtilInit898a90=_UtilInit898a90")
extern "C" {
#include "../../../../re/match/s06/0042aab0.c"
}
RH_ScopedInstall(UtilInit898a90, 0x0042aab0);
