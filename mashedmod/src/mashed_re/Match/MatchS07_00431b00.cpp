// MATCH-S07: port body of 0x00431b00 = the byte-matched source re/match/s07/00431b00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/BootLowRvaCluster.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00431b00 DefaultParam_SetField00
#pragma comment(linker, "/EXPORT:DefaultParam_SetField00=_DefaultParam_SetField00")
extern "C" {
#include "../../../../re/match/s07/00431b00.c"
}
RH_ScopedInstall(DefaultParam_SetField00, 0x00431b00);
