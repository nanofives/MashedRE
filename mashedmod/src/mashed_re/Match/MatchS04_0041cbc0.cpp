// MATCH-S04: port body of 0x0041cbc0 = the byte-matched source re/match/s04/0041cbc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/TimerInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041cbc0 FloatTableInit
#pragma comment(linker, "/EXPORT:FloatTableInit=_FloatTableInit")
extern "C" {
#include "../../../../re/match/s04/0041cbc0.c"
}
RH_ScopedInstall(FloatTableInit, 0x0041cbc0);
