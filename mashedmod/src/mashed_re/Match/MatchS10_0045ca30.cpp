// MATCH-S10: port body of 0x0045ca30 = the byte-matched source re/match/s10/0045ca30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, --bytes BYTE-IDENTICAL).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp
// by MATCH-FIX-45CA30: the old body walked 6 floats, the original walks all 36 at 0x0088f5e0..0x0088f670).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045ca30 FloatClamp3Level45ca30
#pragma comment(linker, "/EXPORT:FloatClamp3Level45ca30=_FloatClamp3Level45ca30")
extern "C" {
#include "../../../../re/match/s10/0045ca30.c"
}
RH_ScopedInstall(FloatClamp3Level45ca30, 0x0045ca30);
