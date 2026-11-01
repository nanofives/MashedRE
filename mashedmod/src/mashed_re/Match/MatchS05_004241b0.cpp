// MATCH-S05: port body of 0x004241b0 = the byte-matched source re/match/s05/004241b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_s3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004241b0 GetDat008994c0
#pragma comment(linker, "/EXPORT:GetDat008994c0=_GetDat008994c0")
extern "C" {
#include "../../../../re/match/s05/004241b0.c"
}
RH_ScopedInstall(GetDat008994c0, 0x004241b0);
