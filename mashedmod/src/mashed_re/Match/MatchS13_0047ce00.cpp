// MATCH-S13: port body of 0x0047ce00 = the byte-matched source re/match/s13/0047ce00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round55.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047ce00 Table6c9438Get
#pragma comment(linker, "/EXPORT:Table6c9438Get=_Table6c9438Get")
extern "C" {
#include "../../../../re/match/s13/0047ce00.c"
}
RH_ScopedInstall(Table6c9438Get, 0x0047ce00);
