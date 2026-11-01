// MATCH-S05: port body of 0x00422b30 = the byte-matched source re/match/s05/00422b30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/TimerReset.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00422b30 TimerArrayClear
#pragma comment(linker, "/EXPORT:TimerArrayClear=_TimerArrayClear")
extern "C" {
#include "../../../../re/match/s05/00422b30.c"
}
RH_ScopedInstall(TimerArrayClear, 0x00422b30);
