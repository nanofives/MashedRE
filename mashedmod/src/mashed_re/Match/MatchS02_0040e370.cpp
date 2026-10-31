// MATCH-S02: port body of 0x0040e370 = the byte-matched source re/match/s02/0040e370.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilLeaves.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e370 IsCarSlotActive
#pragma comment(linker, "/EXPORT:IsCarSlotActive=_IsCarSlotActive")
extern "C" {
#include "../../../../re/match/s02/0040e370.c"
}
RH_ScopedInstall(IsCarSlotActive, 0x0040e370);
