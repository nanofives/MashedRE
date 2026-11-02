// MATCH-S06: port body of 0x00427990 = the byte-matched source re/match/s06/00427990.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_00427780 FontText_StringTableLookup
#define FUN_004277a0 TextCtrlCodeRemapBody
#define FUN_005555b0 MatchS06_00427990_Call005555b0
#define FUN_00556e90 MatchS06_00427990_Call00556e90
// Callees with no port body in either target: raw-address forwarders (S04 rule), the call the original makes.
extern "C" int MatchS06_00427990_Call005555b0(int a, short *text, float size, float *pos, int align, int f) {
    return reinterpret_cast<int (__cdecl*)(int, short *, float, float *, int, int)>(0x005555b0u)(a, text, size, pos, align, f);
}
extern "C" float * MatchS06_00427990_Call00556e90(int a, void *b, void *c, void *d, void *e) {
    return reinterpret_cast<float * (__cdecl*)(int, void *, void *, void *, void *)>(0x00556e90u)(a, b, c, d, e);
}
// FUN_00552d10: the port body FontMatrix_Push (HUD/FontCtx.cpp) is .asi-only. The exe has no body of 0x00552d10,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" int MatchS06_00427990_Call00552d10(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00552d10u)();
}
#define FUN_00552d10 MatchS06_00427990_Call00552d10
#else
#define FUN_00552d10 FontMatrix_Push
#endif
// FUN_00552d70: the port body ViewportStackPop (Render/HighAB3Helpers_p6.cpp) is .asi-only. The exe has no body of 0x00552d70,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" void MatchS06_00427990_Call00552d70(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00552d70u)();
}
#define FUN_00552d70 MatchS06_00427990_Call00552d70
#else
#define FUN_00552d70 ViewportStackPop
#endif
#pragma comment(linker, "/EXPORT:FUN_00427990=_FUN_00427990")
extern "C" {
#include "../../../../re/match/s06/00427990.c"
}
