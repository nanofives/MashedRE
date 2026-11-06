// MATCH-S11: port body of 0x00473870 = the byte-matched source re/match/s11/00473870.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/DrawQuadPrimitives.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00473870 TextSpriteUVExplicit
#pragma comment(linker, "/EXPORT:TextSpriteUVExplicit=_TextSpriteUVExplicit")
extern "C" {
#include "../../../../re/match/s11/00473870.c"
}
RH_ScopedInstall(TextSpriteUVExplicit, 0x00473870);
