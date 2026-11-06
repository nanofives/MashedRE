// MATCH-S14: port body of 0x00488390 = the byte-matched source re/match/s14/00488390.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round50.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00488390 Table86ae38Set
#pragma comment(linker, "/EXPORT:Table86ae38Set=_Table86ae38Set")
extern "C" {
#include "../../../../re/match/s14/00488390.c"
}
RH_ScopedInstall(Table86ae38Set, 0x00488390);
