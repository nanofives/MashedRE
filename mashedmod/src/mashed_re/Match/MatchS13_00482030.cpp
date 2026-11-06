// MATCH-S13: port body of 0x00482030 = the byte-matched source re/match/s13/00482030.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Vehicle/SubStripUV.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00482030 SubStripQuadUV
#pragma comment(linker, "/EXPORT:SubStripQuadUV=_SubStripQuadUV")
extern "C" {
#include "../../../../re/match/s13/00482030.c"
}
RH_ScopedInstall(SubStripQuadUV, 0x00482030);
