// MATCH-S07: port body of 0x0042c280 = the byte-matched source re/match/s07/0042c280.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042c280 Init42c280
#define FUN_0042bf30 Post0042bf30
// Callee of the matched source: 0x0042bf30's port body is Post0042bf30 (Match/MatchS07_0042bf30.cpp, the matched
// source re/match/s07/0042bf30.c). The raw-address forwarder that stood here until it had one is gone.
#pragma comment(linker, "/EXPORT:Init42c280=_Init42c280")
extern "C" {
#include "../../../../re/match/s07/0042c280.c"
}
RH_ScopedInstall(Init42c280, 0x0042c280);
