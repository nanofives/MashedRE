// MATCH-S06: port body of 0x0042a470 = the byte-matched source re/match/s06/0042a470.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callee 0x004b5350: no port body, called at its original address.

#define MATCH_PORT 1
#define FUN_004b5350 MatchS06_0042a470_Call004b5350
extern "C" int MatchS06_0042a470_Call004b5350(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b5350u)(p0);
}
#pragma comment(linker, "/EXPORT:FUN_0042a470=_FUN_0042a470")
extern "C" {
#include "../../../../re/match/s06/0042a470.c"
}
