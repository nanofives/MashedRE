// MATCH-S06: port body of 0x00427ca0 = the byte-matched source re/match/s06/00427ca0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_00556cc0 SetDat00912a20
#define FUN_00556d70 MatchS06_00427ca0_Call00556d70
#define FUN_00556e90 MatchS06_00427ca0_Call00556e90
// Callees with no port body in either target: raw-address forwarders (S04 rule), the call the original makes.
extern "C" int MatchS06_00427ca0_Call00556d70(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00556d70u)();
}
extern "C" int MatchS06_00427ca0_Call00556e90(int font, void *c0, void *c1, void *c2, void *c3) {
    return reinterpret_cast<int (__cdecl*)(int, void *, void *, void *, void *)>(0x00556e90u)(font, c0, c1, c2, c3);
}
// FUN_00552b60: the port body FontSys_InitSeq (HUD/FontCtx.cpp) is .asi-only. The exe has no body of 0x00552b60,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" void MatchS06_00427ca0_Call00552b60(int p) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00552b60u)(p);
}
#define FUN_00552b60 MatchS06_00427ca0_Call00552b60
#else
#define FUN_00552b60 FontSys_InitSeq
#endif
#pragma comment(linker, "/EXPORT:FUN_00427ca0=_FUN_00427ca0")
extern "C" {
#include "../../../../re/match/s06/00427ca0.c"
}
