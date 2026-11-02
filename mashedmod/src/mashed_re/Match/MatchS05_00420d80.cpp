// MATCH-S05: port body of 0x00420d80 = the byte-matched source re/match/s05/00420d80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00420d80 IndexedFloatPairSum420d80
#pragma comment(linker, "/EXPORT:IndexedFloatPairSum420d80=_IndexedFloatPairSum420d80")
extern "C" {
#include "../../../../re/match/s05/00420d80.c"
}
RH_ScopedInstall(IndexedFloatPairSum420d80, 0x00420d80);
