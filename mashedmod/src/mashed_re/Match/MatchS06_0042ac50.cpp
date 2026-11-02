// MATCH-S06: port body of 0x0042ac50 = the byte-matched source re/match/s06/0042ac50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// The matched function is `static` (n in EAX, w in ECX under the DDK); under VS2022 it is cdecl Calc42ac50(n, w)
// (MATCH_STATIC), the old body's ABI. Not installed, as before.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_0042ac50 Calc42ac50
#pragma comment(linker, "/EXPORT:Calc42ac50=_Calc42ac50")
extern "C" {
#include "../../../../re/match/s06/0042ac50.c"
}
