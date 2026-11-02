// MATCH-S06: port body of 0x00426bd0 = the byte-matched source re/match/s06/00426bd0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/GlobalGetters_s5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426bd0 GetTableEntry0066d658
#pragma comment(linker, "/EXPORT:GetTableEntry0066d658=_GetTableEntry0066d658")
extern "C" {
#include "../../../../re/match/s06/00426bd0.c"
}
RH_ScopedInstall(GetTableEntry0066d658, 0x00426bd0);
