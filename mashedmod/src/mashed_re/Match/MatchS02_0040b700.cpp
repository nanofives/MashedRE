// MATCH-S02: port body of 0x0040b700 = the byte-matched source re/match/s02/0040b700.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// New body (no earlier port body, so no install: the .asi keeps the original at 0x0040b700).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0040b700 Counters8a9530Inc
#define FUN_0040e340 GetLiveCarCount
#define FUN_0042f500 GetDat0067ea64
// Callee 0x0040e340: GetLiveCarCount (Match/MatchS02_0040e340.cpp, both targets).
// Callee 0x0042f500: GetDat0067ea64 (Match/MatchS07_0042f500.cpp, both targets).
#pragma comment(linker, "/EXPORT:Counters8a9530Inc=_Counters8a9530Inc")
extern "C" {
#include "../../../../re/match/s02/0040b700.c"
}
