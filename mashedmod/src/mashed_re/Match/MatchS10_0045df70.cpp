// MATCH-S10: port body of 0x0045df70 = the byte-matched source re/match/s10/0045df70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045df70 FloatStep45df70
#pragma comment(linker, "/EXPORT:FloatStep45df70=_FloatStep45df70")
extern "C" {
#include "../../../../re/match/s10/0045df70.c"
}
RH_ScopedInstall(FloatStep45df70, 0x0045df70);
