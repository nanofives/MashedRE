// MATCH-S12: port body of 0x00477b40 = the byte-matched source re/match/s12/00477b40.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00477b40 ZeroTwoRegions477b40
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:ZeroTwoRegions477b40=_ZeroTwoRegions477b40")
extern "C" {
#include "../../../../re/match/s12/00477b40.c"
}
RH_ScopedInstall(ZeroTwoRegions477b40, 0x00477b40);
