// MATCH-GFFU: port body of 0x00417740 = the byte-matched source re/match/gffu/00417740.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// New body (no earlier port body, so no install: the .asi keeps the original at 0x00417740).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00417740 Float89a870ToInt
#pragma comment(linker, "/EXPORT:Float89a870ToInt=_Float89a870ToInt")
extern "C" {
#include "../../../../re/match/gffu/00417740.c"
}
