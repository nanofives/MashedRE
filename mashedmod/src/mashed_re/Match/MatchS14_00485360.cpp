// MATCH-S14: port body of 0x00485360 = the byte-matched source re/match/s14/00485360.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round17.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00485360 DynObjListGetCount
#pragma comment(linker, "/EXPORT:DynObjListGetCount=_DynObjListGetCount")
extern "C" {
#include "../../../../re/match/s14/00485360.c"
}
RH_ScopedInstall(DynObjListGetCount, 0x00485360);
