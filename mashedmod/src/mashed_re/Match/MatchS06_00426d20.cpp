// MATCH-S06: port body of 0x00426d20 = the byte-matched source re/match/s06/00426d20.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x00426d20 had no port body: this is its first body (new body, top-down rule L-72). No install; caller 0x004270f0.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004c1340 MatchS06_00426d20_Call004c1340
#define FUN_004c1520 MatchS06_00426d20_Call004c1520
#define FUN_004c15c0 MatchS06_00426d20_Call004c15c0
#pragma comment(linker, "/EXPORT:FUN_00426d20=_FUN_00426d20")
extern "C" {
#include "../../../../re/match/s06/00426d20.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
extern "C" {
int MatchS06_00426d20_Call004c1340(int p0, int p1, int p2) {
    return reinterpret_cast<int (__cdecl*)(int, int, int)>(0x004c1340u)(p0, p1, p2);
}
int MatchS06_00426d20_Call004c1520(int p0, float *p1, int p2, int p3) {
    return reinterpret_cast<int (__cdecl*)(int, float *, int, int)>(0x004c1520u)(p0, p1, p2, p3);
}
void MatchS06_00426d20_Call004c15c0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004c15c0u)(p0);
}
}
