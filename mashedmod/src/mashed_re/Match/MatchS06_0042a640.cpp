// MATCH-S06: port body of 0x0042a640 = the byte-matched source re/match/s06/0042a640.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuNearLeaves_s6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees: 0x0042a530 = FUN_0042a530 (Match/MatchS06_0042a530.cpp); 0x004b3c60: no port body, called at the original address.
// strchr = the CRT strchr (C linkage).

#define MATCH_PORT 1
#define FUN_0042a640 PathBuilderLoad
#define FUN_004b3c60 MatchS06_0042a640_Call004b3c60
extern "C" int MatchS06_0042a640_Call004b3c60(char * p0) {
    return reinterpret_cast<int (__cdecl*)(char *)>(0x004b3c60u)(p0);
}
#pragma comment(linker, "/EXPORT:PathBuilderLoad=_PathBuilderLoad")
extern "C" {
#include "../../../../re/match/s06/0042a640.c"
}
