// MATCH-S02: port body of 0x0040b6e0 = the byte-matched source re/match/s02/0040b6e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b6e0 Add40b6e0
#define FUN_0042f6a0 GetRaceSubMode
// Callee 0x0042f6a0: GetRaceSubMode (Match/MatchS07_0042f6a0.cpp, both targets); the old naked body called 0x0042f6a0 through its address.
#pragma comment(linker, "/EXPORT:Add40b6e0=_Add40b6e0")
extern "C" {
#include "../../../../re/match/s02/0040b6e0.c"
}
RH_ScopedInstall(Add40b6e0, 0x0040b6e0);
