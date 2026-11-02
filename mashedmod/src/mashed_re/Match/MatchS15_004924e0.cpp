// MATCH-S15: port body of 0x004924e0 = the byte-matched source re/match/s15/004924e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round40.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004924e0 Ret6147b4
#pragma comment(linker, "/EXPORT:Ret6147b4=_Ret6147b4")
extern "C" {
#include "../../../../re/match/s15/004924e0.c"
}
RH_ScopedInstall(Ret6147b4, 0x004924e0);
