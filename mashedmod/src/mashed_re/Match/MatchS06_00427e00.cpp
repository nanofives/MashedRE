// MATCH-S06: port body of 0x00427e00 = the byte-matched source re/match/s06/00427e00.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the port had no body of this RVA).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_00427780 FontText_StringTableLookup
#define FUN_004277a0 TextCtrlCodeRemapBody
#define FUN_00556ca0 MatchS06_00427e00_Call00556ca0
#define FUN_00556e90 MatchS06_00427e00_Call00556e90
// Callees with no port body in either target: raw-address forwarders (S04 rule), the call the original makes.
extern "C" void MatchS06_00427e00_Call00556ca0(int font, short *text, float size, float *pos, int font2) {
    reinterpret_cast<void (__cdecl*)(int, short *, float, float *, int)>(0x00556ca0u)(font, text, size, pos, font2);
}
extern "C" int MatchS06_00427e00_Call00556e90(int font, void *c0, void *c1, void *c2, void *c3) {
    return reinterpret_cast<int (__cdecl*)(int, void *, void *, void *, void *)>(0x00556e90u)(font, c0, c1, c2, c3);
}
// FUN_00552750: the port body FontCtx_ResetTransform (HUD/TextCluster.cpp) is .asi-only. The exe has no body of 0x00552750,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" int MatchS06_00427e00_Call00552750(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00552750u)();
}
#define FUN_00552750 MatchS06_00427e00_Call00552750
#else
#define FUN_00552750 FontCtx_ResetTransform
#endif
// FUN_00552d10: the port body FontMatrix_Push (HUD/FontCtx.cpp) is .asi-only. The exe has no body of 0x00552d10,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" int MatchS06_00427e00_Call00552d10(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00552d10u)();
}
#define FUN_00552d10 MatchS06_00427e00_Call00552d10
#else
#define FUN_00552d10 FontMatrix_Push
#endif
// FUN_00552d70: the port body ViewportStackPop (Render/HighAB3Helpers_p6.cpp) is .asi-only. The exe has no body of 0x00552d70,
// so there it is a raw-address forwarder (S04 rule).
#ifdef MASHED_STANDALONE
extern "C" void MatchS06_00427e00_Call00552d70(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00552d70u)();
}
#define FUN_00552d70 MatchS06_00427e00_Call00552d70
#else
#define FUN_00552d70 ViewportStackPop
#endif
#pragma comment(linker, "/EXPORT:FUN_00427e00=_FUN_00427e00")
extern "C" {
#include "../../../../re/match/s06/00427e00.c"
}
