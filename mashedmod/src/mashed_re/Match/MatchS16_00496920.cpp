// MATCH-S16: port body of 0x00496920 = the byte-matched source re/match/s16/00496920.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round17.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00496920 TimerTable772ffcGet
#pragma comment(linker, "/EXPORT:TimerTable772ffcGet=_TimerTable772ffcGet")
extern "C" {
#include "../../../../re/match/s16/00496920.c"
}
RH_ScopedInstall(TimerTable772ffcGet, 0x00496920);
