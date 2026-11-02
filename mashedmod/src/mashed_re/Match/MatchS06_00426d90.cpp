// MATCH-S06: port body of 0x00426d90 = the byte-matched source re/match/s06/00426d90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuNearLeaves_s6.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callee 0x004e6680: no port body; the old body called it through its original address. No install (none before).

#define MATCH_PORT 1
#define FUN_00426d90 HandleArrayRelease
#define FUN_004e6680 MatchS06_00426d90_Call004e6680
extern "C" int MatchS06_00426d90_Call004e6680(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004e6680u)(p0);
}
#pragma comment(linker, "/EXPORT:HandleArrayRelease=_HandleArrayRelease")
extern "C" {
#include "../../../../re/match/s06/00426d90.c"
}
