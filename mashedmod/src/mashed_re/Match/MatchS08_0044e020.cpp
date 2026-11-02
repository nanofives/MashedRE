// MATCH-S08: port body of 0x0044e020 = the byte-matched source re/match/s08/0044e020.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round75.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0044e020 FloatLt44e020
#pragma comment(linker, "/EXPORT:FloatLt44e020=_FloatLt44e020")
extern "C" {
#include "../../../../re/match/s08/0044e020.c"
}
RH_ScopedInstall(FloatLt44e020, 0x0044e020);
