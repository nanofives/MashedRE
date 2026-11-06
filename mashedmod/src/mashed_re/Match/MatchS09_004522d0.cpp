// MATCH-S09: port body of 0x004522d0 = the byte-matched source re/match/s09/004522d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Audio/MixedC3Sweep.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_004522d0 RwGlobalsFree4522d0
#pragma comment(linker, "/EXPORT:RwGlobalsFree4522d0=_RwGlobalsFree4522d0")
extern "C" {
#include "../../../../re/match/s09/004522d0.c"
}
