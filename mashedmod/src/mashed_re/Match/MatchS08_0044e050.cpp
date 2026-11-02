// MATCH-S08: port body of 0x0044e050 = the byte-matched source re/match/s08/0044e050.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round43.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0044e050 FloatTable8900a8Get
#pragma comment(linker, "/EXPORT:FloatTable8900a8Get=_FloatTable8900a8Get")
extern "C" {
#include "../../../../re/match/s08/0044e050.c"
}
RH_ScopedInstall(FloatTable8900a8Get, 0x0044e050);
