// MATCH-S07: port body of 0x0042d390 = the byte-matched source re/match/s07/0042d390.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round32.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042d390 Global67ea6cGet
#pragma comment(linker, "/EXPORT:Global67ea6cGet=_Global67ea6cGet")
extern "C" {
#include "../../../../re/match/s07/0042d390.c"
}
RH_ScopedInstall(Global67ea6cGet, 0x0042d390);
