// MATCH-S06: port body of 0x0042a7f0 = the byte-matched source re/match/s06/0042a7f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a530 = FUN_0042a530 (Match/MatchS06_0042a530.cpp); 0x004b3cc0: no port body, called at the original address.
// strchr = the CRT strchr (C linkage).

#define MATCH_PORT 1
#define FUN_004b3cc0 MatchS06_0042a7f0_Call004b3cc0
extern "C" int MatchS06_0042a7f0_Call004b3cc0(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b3cc0u)(p0);
}
#pragma comment(linker, "/EXPORT:FUN_0042a7f0=_FUN_0042a7f0")
extern "C" {
#include "../../../../re/match/s06/0042a7f0.c"
}
