// MATCH-S06: port body of 0x004260a0 = the byte-matched source re/match/s06/004260a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/GlobalGetters_s5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004260a0 GetDat00657438
#pragma comment(linker, "/EXPORT:GetDat00657438=_GetDat00657438")
extern "C" {
#include "../../../../re/match/s06/004260a0.c"
}
RH_ScopedInstall(GetDat00657438, 0x004260a0);
