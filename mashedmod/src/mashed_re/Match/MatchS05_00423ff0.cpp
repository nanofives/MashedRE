// MATCH-S05: port body of 0x00423ff0 = the byte-matched source re/match/s05/00423ff0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_s3.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00423ff0 PlayerBlock2Field50Get
#define FUN_0042f500 GetDat0067ea64
#pragma comment(linker, "/EXPORT:PlayerBlock2Field50Get=_PlayerBlock2Field50Get")
extern "C" {
#include "../../../../re/match/s05/00423ff0.c"
}
RH_ScopedInstall(PlayerBlock2Field50Get, 0x00423ff0);
