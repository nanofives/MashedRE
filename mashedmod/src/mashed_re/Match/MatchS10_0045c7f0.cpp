// MATCH-S10: port body of 0x0045c7f0 = the byte-matched source re/match/s10/0045c7f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045c7f0 Sub45c7f0
#define FUN_0040e350 GetRenderSubMode
#define FUN_0040e450 Flag63b908Get
#pragma comment(linker, "/EXPORT:Sub45c7f0=_Sub45c7f0")
extern "C" {
#include "../../../../re/match/s10/0045c7f0.c"
}
RH_ScopedInstall(Sub45c7f0, 0x0045c7f0);
