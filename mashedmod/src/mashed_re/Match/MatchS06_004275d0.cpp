// MATCH-S06: port body of 0x004275d0 = the byte-matched source re/match/s06/004275d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body: the RVA had no port body).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees with a port body: 0x00552d10 FontMatrix_Push (HUD/FontCtx.cpp), 0x00552750 FontCtx_ResetTransform
// (HUD/TextCluster.cpp), 0x00552d70 ViewportStackPop (Render/HighAB3Helpers_p6.cpp), 0x00427580
// FrontendQuadParamInit427580 (Match/MatchS06_00427580.cpp). 0x00552a60 / 0x00553ef0 / 0x00552920 / 0x00553cf0 /
// 0x00553e80: no port body, called at their original addresses.

#define MATCH_PORT 1
#define FUN_00552d10 FontMatrix_Push
#define FUN_00552750 FontCtx_ResetTransform
#define FUN_00552d70 ViewportStackPop
#define FUN_00427580 FrontendQuadParamInit427580
#define FUN_00552a60 MatchS06_004275d0_Call00552a60
#define FUN_00553ef0 MatchS06_004275d0_Call00553ef0
#define FUN_00552920 MatchS06_004275d0_Call00552920
#define FUN_00553cf0 MatchS06_004275d0_Call00553cf0
#define FUN_00553e80 MatchS06_004275d0_Call00553e80
extern "C" unsigned int MatchS06_004275d0_Call00552a60(int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int)>(0x00552a60u)(p0);
}
extern "C" int * MatchS06_004275d0_Call00553ef0(void) {
    return reinterpret_cast<int * (__cdecl*)()>(0x00553ef0u)();
}
extern "C" unsigned int MatchS06_004275d0_Call00552920(int * p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(int *)>(0x00552920u)(p0);
}
extern "C" int * MatchS06_004275d0_Call00553cf0(int * p0, void * p1) {
    return reinterpret_cast<int * (__cdecl*)(int *, void *)>(0x00553cf0u)(p0, p1);
}
extern "C" bool MatchS06_004275d0_Call00553e80(int * p0) {
    return reinterpret_cast<bool (__cdecl*)(int *)>(0x00553e80u)(p0);
}
#pragma comment(linker, "/EXPORT:FUN_004275d0=_FUN_004275d0")
extern "C" {
#include "../../../../re/match/s06/004275d0.c"
}
