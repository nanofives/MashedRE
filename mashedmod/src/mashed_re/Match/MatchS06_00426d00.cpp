// MATCH-S06: port body of 0x00426d00 = the byte-matched source re/match/s06/00426d00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s6.cpp and Race/PathProgStep_naked.inc PP_426d00).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426d00 FrontendArraySlotGet
#pragma comment(linker, "/EXPORT:FrontendArraySlotGet=_FrontendArraySlotGet")
extern "C" {
#include "../../../../re/match/s06/00426d00.c"
}
RH_ScopedInstall(FrontendArraySlotGet, 0x00426d00);
