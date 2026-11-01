// MATCH-S05: port body of 0x00420da0 = the byte-matched source re/match/s05/00420da0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round59.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00420da0 TblRetPtrout63e4b8_0
#pragma comment(linker, "/EXPORT:TblRetPtrout63e4b8_0=_TblRetPtrout63e4b8_0")
extern "C" {
#include "../../../../re/match/s05/00420da0.c"
}
RH_ScopedInstall(TblRetPtrout63e4b8_0, 0x00420da0);
