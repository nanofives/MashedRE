// MATCH-S10: port body of 0x00462760 = the byte-matched source re/match/s10/00462760.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00462760 Pred462760
#define FUN_005a89a0 VoiceFieldD6cGet
#define FUN_005a89b0 VoiceFieldD70Get
#pragma comment(linker, "/EXPORT:Pred462760=_Pred462760")
extern "C" {
#include "../../../../re/match/s10/00462760.c"
}
RH_ScopedInstall(Pred462760, 0x00462760);
