// MATCH-S06: port body of 0x0042a8d0 = the byte-matched source re/match/s06/0042a8d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#pragma comment(linker, "/EXPORT:FUN_0042a8d0=_FUN_0042a8d0")
extern "C" {
#include "../../../../re/match/s06/0042a8d0.c"
}
