// MATCH-S16: port body of 0x004944b0 = the byte-matched source re/match/s16/004944b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004944b0 Zero4944b0
// __fastcall (ECX in, EAX out, as the original): the decorated name is @Zero4944b0@4.
#pragma comment(linker, "/EXPORT:Zero4944b0=@Zero4944b0@4")
extern "C" {
#include "../../../../re/match/s16/004944b0.c"
}
RH_ScopedInstall(Zero4944b0, 0x004944b0);
