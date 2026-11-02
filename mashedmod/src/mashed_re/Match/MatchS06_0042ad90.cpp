// MATCH-S06: port body of 0x0042ad90 = the byte-matched source re/match/s06/0042ad90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/KvFind.cpp).
// The matched function is `static` (EDX/EBX/EDI under the DDK); under VS2022 it is cdecl Find42ad90(list, key, nth)
// (MATCH_STATIC), the old body's ABI. The install stays on Util/PromoLoop_sessionB.cpp Find42ad90_RegAbi, which calls it.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_0042ad90 Find42ad90
#pragma comment(linker, "/EXPORT:Find42ad90=_Find42ad90")
extern "C" {
#include "../../../../re/match/s06/0042ad90.c"
}
