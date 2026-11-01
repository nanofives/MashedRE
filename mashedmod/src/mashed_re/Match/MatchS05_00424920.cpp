// MATCH-S05: port body of 0x00424920 = the byte-matched source re/match/s05/00424920.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuMiscLeaves_t2.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00424920 EndOfRoundAccumulator
#pragma comment(linker, "/EXPORT:EndOfRoundAccumulator=_EndOfRoundAccumulator")
extern "C" {
#include "../../../../re/match/s05/00424920.c"
}
RH_ScopedInstall(EndOfRoundAccumulator, 0x00424920);
