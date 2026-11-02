// MATCH-S15: port body of 0x00493f80 = the byte-matched source re/match/s15/00493f80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00493f80 IntroVideoDimGetter
#pragma comment(linker, "/EXPORT:IntroVideoDimGetter=_IntroVideoDimGetter")
extern "C" {
#include "../../../../re/match/s15/00493f80.c"
}
RH_ScopedInstall(IntroVideoDimGetter, 0x00493f80);
