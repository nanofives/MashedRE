// MATCH-S06: port body of 0x00429a80 = the byte-matched source re/match/s06/00429a80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuScoreGetters.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429a80 LapLapsGetBySlot
#pragma comment(linker, "/EXPORT:LapLapsGetBySlot=_LapLapsGetBySlot")
extern "C" {
#include "../../../../re/match/s06/00429a80.c"
}
RH_ScopedInstall(LapLapsGetBySlot, 0x00429a80);
