// MATCH-S06: port body of 0x004260b0 = the byte-matched source re/match/s06/004260b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/GlobalGetters_s5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004260b0 GetDat0065743c
#pragma comment(linker, "/EXPORT:GetDat0065743c=_GetDat0065743c")
extern "C" {
#include "../../../../re/match/s06/004260b0.c"
}
RH_ScopedInstall(GetDat0065743c, 0x004260b0);
