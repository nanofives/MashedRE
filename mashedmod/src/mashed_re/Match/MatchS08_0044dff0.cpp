// MATCH-S08: port body of 0x0044dff0 = the byte-matched source re/match/s08/0044dff0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0044dff0 ItemWorldPos44dff0
#pragma comment(linker, "/EXPORT:ItemWorldPos44dff0=_ItemWorldPos44dff0")
extern "C" {
#include "../../../../re/match/s08/0044dff0.c"
}
RH_ScopedInstall(ItemWorldPos44dff0, 0x0044dff0);
