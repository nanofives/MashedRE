// MATCH-S11: port body of 0x0046bd20 = the byte-matched source re/match/s11/0046bd20.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round56.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0046bd20 VehTbl8820a0Get3
#pragma comment(linker, "/EXPORT:VehTbl8820a0Get3=_VehTbl8820a0Get3")
extern "C" {
#include "../../../../re/match/s11/0046bd20.c"
}
RH_ScopedInstall(VehTbl8820a0Get3, 0x0046bd20);
