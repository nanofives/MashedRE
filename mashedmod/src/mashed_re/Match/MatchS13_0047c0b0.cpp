// MATCH-S13: port body of 0x0047c0b0 = the byte-matched source re/match/s13/0047c0b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0047c0b0 ClearColl47c0b0
#pragma comment(linker, "/EXPORT:ClearColl47c0b0=_ClearColl47c0b0")
extern "C" {
#include "../../../../re/match/s13/0047c0b0.c"
}
RH_ScopedInstall(ClearColl47c0b0, 0x0047c0b0);
