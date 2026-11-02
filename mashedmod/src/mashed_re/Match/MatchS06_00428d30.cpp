// MATCH-S06: port body of 0x00428d30 = the byte-matched source re/match/s06/00428d30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00401ee0 -> EntityTableSelectUpdate (Match/MatchS01.cpp)
//   0x0040d250 -> DoubleDerefIndexedGetter (Match/MatchS02_0040d250.cpp)
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x00428760 -> ViewportScaledRectDraw_Arg7Zero (symmap swap-batch2 sw5)
//   0x0042f0b0 -> GetFrameCounterPlus73 (Frontend/MenuInit.cpp)
//   0x00472c60 -> ChromeBaseDraw (Frontend/DrawQuadPrimitives.cpp)
//   0x00472f40 -> TextGradientV0V1Override (Frontend/DrawQuadPrimitives.cpp)
//   0x004730b0 -> TextGradientV2V3Override (Frontend/DrawQuadPrimitives.cpp)
//   0x004c19f0 -> RwVtableSlot07Call (Boot/FrameDispatch.cpp)
//   0x00428450 -> HudSpinCoinAnim in the .asi (HUD/TextCluster.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1a00 -> IntroSplashVtableSlot6 in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1bb0 -> IntroSplashRenderState in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1c80 -> ViewportDimsSet in the .asi (HUD/ViewportDimsSet_ag.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0042e590 -> raw-address forwarder: port body Frontend/SpriteDispatch.cpp SpriteAnimFrameThunk is void with 9 args (incompatible with int(int)); in the .asi the address carries its JMP
//   0x004671a0 -> raw-address forwarder: no port body (Frontend/IntroSplash.cpp OrigVehicle0Get is itself a raw call-through)
//   0x00473220 -> raw-address forwarder: no port body
//   0x004744a0 -> raw-address forwarder: no port body
//   0x004c1be0 -> raw-address forwarder: no port body (IntroSplash.cpp OrigRenderTargetHelper is a raw call-through)

#define MATCH_PORT 1
#define FUN_00428d30 FUN_00428d30
#define FUN_00401ee0 EntityTableSelectUpdate
#define FUN_0040d250 DoubleDerefIndexedGetter
#define FUN_00427e00 FUN_00427e00
#define FUN_00428760 ViewportScaledRectDraw_Arg7Zero
#define FUN_0042f0b0 GetFrameCounterPlus73
#define FUN_00472c60 ChromeBaseDraw
#define FUN_00472f40 TextGradientV0V1Override
#define FUN_004730b0 TextGradientV2V3Override
#define FUN_004c19f0 RwVtableSlot07Call
#ifdef MASHED_STANDALONE
#define FUN_00428450 MatchS06_00428d30_Call00428450
#else
#define FUN_00428450 HudSpinCoinAnim
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1a00 MatchS06_00428d30_Call004c1a00
#else
#define FUN_004c1a00 IntroSplashVtableSlot6
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1bb0 MatchS06_00428d30_Call004c1bb0
#else
#define FUN_004c1bb0 IntroSplashRenderState
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1c80 MatchS06_00428d30_Call004c1c80
#else
#define FUN_004c1c80 ViewportDimsSet
#endif
#define FUN_0042e590 MatchS06_00428d30_Call0042e590
#define FUN_004671a0 MatchS06_00428d30_Call004671a0
#define FUN_00473220 MatchS06_00428d30_Call00473220
#define FUN_004744a0 MatchS06_00428d30_Call004744a0
#define FUN_004c1be0 MatchS06_00428d30_Call004c1be0
#pragma comment(linker, "/EXPORT:FUN_00428d30=_FUN_00428d30")
extern "C" {
#include "../../../../re/match/s06/00428d30.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
int MatchS06_00428d30_Call0042e590(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x0042e590u)(p0);
}
int MatchS06_00428d30_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
void MatchS06_00428d30_Call00473220(S06_V2d pos, S06_V2d size, S06_RGBA c0, S06_RGBA c1) {
    reinterpret_cast<void (__cdecl*)(S06_V2d, S06_V2d, S06_RGBA, S06_RGBA)>(0x00473220u)(pos, size, c0, c1);
}
void MatchS06_00428d30_Call004744a0(S06_V2d pos, S06_V2d size, int p4, double p5, double p6, int p7, int p8) {
    reinterpret_cast<void (__cdecl*)(S06_V2d, S06_V2d, int, double, double, int, int)>(0x004744a0u)(pos, size, p4, p5, p6, p7, p8);
}
void MatchS06_00428d30_Call004c1be0(int font, int p1, int p2) {
    reinterpret_cast<void (__cdecl*)(int, int, int)>(0x004c1be0u)(font, p1, p2);
}
#ifdef MASHED_STANDALONE
void MatchS06_00428d30_Call00428450(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x00428450u)(p0, p1);
}
int MatchS06_00428d30_Call004c1a00(int font) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004c1a00u)(font);
}
void MatchS06_00428d30_Call004c1bb0(int font, S06_RGBA *color, int p2) {
    reinterpret_cast<void (__cdecl*)(int, S06_RGBA *, int)>(0x004c1bb0u)(font, color, p2);
}
void MatchS06_00428d30_Call004c1c80(int font, S06_V2d *size) {
    reinterpret_cast<void (__cdecl*)(int, S06_V2d *)>(0x004c1c80u)(font, size);
}
#endif
}
