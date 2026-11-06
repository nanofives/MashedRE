// MATCH-S16: port body of 0x00494f30 = the byte-matched source re/match/s16/00494f30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/SplashGameMode_t5.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00494f30 AspectRatioSnapshot
#define FUN_00493fc0 AspectRatioGlobalGet
#pragma comment(linker, "/EXPORT:AspectRatioSnapshot=_AspectRatioSnapshot")
extern "C" {
#include "../../../../re/match/s16/00494f30.c"
}
RH_ScopedInstall(AspectRatioSnapshot, 0x00494f30);
