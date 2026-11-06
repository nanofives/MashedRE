// MATCH-S10: port body of 0x0045c860 = the byte-matched source re/match/s10/0045c860.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round55.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045c860 Clear88f0a0x4
#pragma comment(linker, "/EXPORT:Clear88f0a0x4=_Clear88f0a0x4")
extern "C" {
#include "../../../../re/match/s10/0045c860.c"
}
RH_ScopedInstall(Clear88f0a0x4, 0x0045c860);
