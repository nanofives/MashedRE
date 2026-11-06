// MATCH-S15: port body of 0x00492340 = the byte-matched source re/match/s15/00492340.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/GameModeInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00492340 CarSlotInit
#pragma comment(linker, "/EXPORT:CarSlotInit=_CarSlotInit")
extern "C" {
#include "../../../../re/match/s15/00492340.c"
}
RH_ScopedInstall(CarSlotInit, 0x00492340);
