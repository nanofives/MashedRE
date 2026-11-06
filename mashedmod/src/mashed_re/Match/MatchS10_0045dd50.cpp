// MATCH-S10: port body of 0x0045dd50 = the byte-matched source re/match/s10/0045dd50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round37.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045dd50 Table8aa300Get
#pragma comment(linker, "/EXPORT:Table8aa300Get=_Table8aa300Get")
extern "C" {
#include "../../../../re/match/s10/0045dd50.c"
}
RH_ScopedInstall(Table8aa300Get, 0x0045dd50);
