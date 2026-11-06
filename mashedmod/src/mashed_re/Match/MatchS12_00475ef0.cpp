// MATCH-S12: port body of 0x00475ef0 = the byte-matched source re/match/s12/00475ef0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Lane2/L2_00475ef0.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00475ef0 L2_FUN_00475ef0
#define FUN_004b6520 ZeroFillWrapper
#pragma comment(linker, "/EXPORT:L2_FUN_00475ef0=_L2_FUN_00475ef0")
extern "C" {
#include "../../../../re/match/s12/00475ef0.c"
}
RH_ScopedInstall(L2_FUN_00475ef0, 0x00475ef0);
