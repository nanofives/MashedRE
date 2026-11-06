// MATCH-S15: port body of 0x00493f70 = the byte-matched source re/match/s15/00493f70.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SplashGameMode_t5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00493f70 VideoStateFlagGet
#pragma comment(linker, "/EXPORT:VideoStateFlagGet=_VideoStateFlagGet")
extern "C" {
#include "../../../../re/match/s15/00493f70.c"
}
RH_ScopedInstall(VideoStateFlagGet, 0x00493f70);
