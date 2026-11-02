// MATCH-S06: port body of 0x004292d0 = the byte-matched source re/match/s06/004292d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x0055deb0 -> RwpWorldSolverHandle (Collision/RwpIntegrator.cpp)

#define MATCH_PORT 1
#define FUN_004292d0 FUN_004292d0
#define FUN_0055deb0 RwpWorldSolverHandle
#pragma comment(linker, "/EXPORT:FUN_004292d0=_FUN_004292d0")
extern "C" {
#include "../../../../re/match/s06/004292d0.c"
}
