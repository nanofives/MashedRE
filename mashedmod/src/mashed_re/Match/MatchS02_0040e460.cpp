// MATCH-S02: port body of 0x0040e460 = the byte-matched source re/match/s02/0040e460.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round26.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040e460 Flag63b908Set
#pragma comment(linker, "/EXPORT:Flag63b908Set=_Flag63b908Set")
extern "C" {
#include "../../../../re/match/s02/0040e460.c"
}
RH_ScopedInstall(Flag63b908Set, 0x0040e460);
