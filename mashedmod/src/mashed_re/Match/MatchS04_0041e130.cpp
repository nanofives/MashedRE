// MATCH-S04: port body of 0x0041e130 = the byte-matched source re/match/s04/0041e130.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/TimerInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041e130 TimerTrackSetter
#pragma comment(linker, "/EXPORT:TimerTrackSetter=_TimerTrackSetter")
extern "C" {
#include "../../../../re/match/s04/0041e130.c"
}
RH_ScopedInstall(TimerTrackSetter, 0x0041e130);
