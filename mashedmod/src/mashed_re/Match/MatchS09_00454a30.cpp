// MATCH-S09: port body of 0x00454a30 = the byte-matched source re/match/s09/00454a30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round47.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00454a30 Table688304Get
#pragma comment(linker, "/EXPORT:Table688304Get=_Table688304Get")
extern "C" {
#include "../../../../re/match/s09/00454a30.c"
}
RH_ScopedInstall(Table688304Get, 0x00454a30);
