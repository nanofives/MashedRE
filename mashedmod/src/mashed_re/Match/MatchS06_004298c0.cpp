// MATCH-S06: port body of 0x004298c0 = the byte-matched source re/match/s06/004298c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round70.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004298c0 Clear67d99c_x4
#pragma comment(linker, "/EXPORT:Clear67d99c_x4=_Clear67d99c_x4")
extern "C" {
#include "../../../../re/match/s06/004298c0.c"
}
RH_ScopedInstall(Clear67d99c_x4, 0x004298c0);
