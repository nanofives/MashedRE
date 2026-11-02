// MATCH-S06: port body of 0x00427c90 = the byte-matched source re/match/s06/00427c90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00427c90 GetDat0067d84c
#pragma comment(linker, "/EXPORT:GetDat0067d84c=_GetDat0067d84c")
extern "C" {
#include "../../../../re/match/s06/00427c90.c"
}
RH_ScopedInstall(GetDat0067d84c, 0x00427c90);
