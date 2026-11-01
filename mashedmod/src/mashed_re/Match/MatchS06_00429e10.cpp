// MATCH-S06: port body of 0x00429e10 = the byte-matched source re/match/s06/00429e10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00405890 -> Pred405890 (Match/MatchS01.cpp)
//   0x00417730 -> VehicleRaceAngleGet (Vehicle/VehicleState.cpp)
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x004295a0 -> HudDualLabelRender (symmap swap-batch2 sw5)
//   0x00429660 -> FUN_00429660 (symmap new-body-batch2 sw5)
//   0x0042f6a0 -> GetRaceSubMode (Util/GameStateGetters.cpp)
//   0x00472c60 -> ChromeBaseDraw (Frontend/DrawQuadPrimitives.cpp)
//   0x004c19f0 -> RwVtableSlot07Call (Boot/FrameDispatch.cpp)
//   0x0040dc80 -> UtilFloat63b910Get in the .asi (Util/PromoLoop_round9.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0040e350 -> GetRenderSubMode in the .asi (Ai/PromoLoop_round1.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0040e4a0 -> ElapsedTimeGet in the .asi (Util/PromoLoop_round26.cpp, .asi-only TU); the exe makes the raw-address call
//   0x0041e140 -> Global63d7e0Get in the .asi (Match/MatchS04_0041e140.cpp, .asi-only TU); the exe makes the raw-address call
//   0x00443080 -> AiTargetEnableGet in the .asi (Util/PromoLoop_round26.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1a00 -> IntroSplashVtableSlot6 in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1bb0 -> IntroSplashRenderState in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1c80 -> ViewportDimsSet in the .asi (HUD/ViewportDimsSet_ag.cpp, .asi-only TU); the exe makes the raw-address call
//   0x00417740 -> raw-address forwarder: no port body
//   0x00429bd0 -> raw-address forwarder: unmatched slice callee, no port body
//   0x004671a0 -> raw-address forwarder: no port body (Frontend/IntroSplash.cpp OrigVehicle0Get is itself a raw call-through)

#define MATCH_PORT 1
#define FUN_00429e10 FUN_00429e10
#define FUN_00405890 Pred405890
#define FUN_00417730 VehicleRaceAngleGet
#define FUN_00427e00 FUN_00427e00
#define FUN_004295a0 HudDualLabelRender
#define FUN_00429660 FUN_00429660
#define FUN_0042f6a0 GetRaceSubMode
#define FUN_00472c60 ChromeBaseDraw
#define FUN_004c19f0 RwVtableSlot07Call
#ifdef MASHED_STANDALONE
#define FUN_0040dc80 MatchS06_00429e10_Call0040dc80
#else
#define FUN_0040dc80 UtilFloat63b910Get
#endif
#ifdef MASHED_STANDALONE
#define FUN_0040e350 MatchS06_00429e10_Call0040e350
#else
#define FUN_0040e350 GetRenderSubMode
#endif
#ifdef MASHED_STANDALONE
#define FUN_0040e4a0 MatchS06_00429e10_Call0040e4a0
#else
#define FUN_0040e4a0 ElapsedTimeGet
#endif
#ifdef MASHED_STANDALONE
#define FUN_0041e140 MatchS06_00429e10_Call0041e140
#else
#define FUN_0041e140 Global63d7e0Get
#endif
#ifdef MASHED_STANDALONE
#define FUN_00443080 MatchS06_00429e10_Call00443080
#else
#define FUN_00443080 AiTargetEnableGet
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1a00 MatchS06_00429e10_Call004c1a00
#else
#define FUN_004c1a00 IntroSplashVtableSlot6
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1bb0 MatchS06_00429e10_Call004c1bb0
#else
#define FUN_004c1bb0 IntroSplashRenderState
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1c80 MatchS06_00429e10_Call004c1c80
#else
#define FUN_004c1c80 ViewportDimsSet
#endif
#define FUN_00417740 MatchS06_00429e10_Call00417740
#define FUN_00429bd0 MatchS06_00429e10_Call00429bd0
#define FUN_004671a0 MatchS06_00429e10_Call004671a0
#pragma comment(linker, "/EXPORT:FUN_00429e10=_FUN_00429e10")
extern "C" {
#include "../../../../re/match/s06/00429e10.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
int MatchS06_00429e10_Call00417740(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x00417740u)(p0);
}
void MatchS06_00429e10_Call00429bd0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00429bd0u)(p0);
}
int MatchS06_00429e10_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
#ifdef MASHED_STANDALONE
float MatchS06_00429e10_Call0040dc80(void) {
    return reinterpret_cast<float (__cdecl*)()>(0x0040dc80u)();
}
int MatchS06_00429e10_Call0040e350(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0040e350u)();
}
int MatchS06_00429e10_Call0040e4a0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0040e4a0u)();
}
int MatchS06_00429e10_Call0041e140(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0041e140u)();
}
int MatchS06_00429e10_Call00443080(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00443080u)();
}
void MatchS06_00429e10_Call004c1a00(int font) {
    reinterpret_cast<void (__cdecl*)(int)>(0x004c1a00u)(font);
}
void MatchS06_00429e10_Call004c1bb0(int font, S06_RGBA *color, int p2) {
    reinterpret_cast<void (__cdecl*)(int, S06_RGBA *, int)>(0x004c1bb0u)(font, color, p2);
}
void MatchS06_00429e10_Call004c1c80(int font, S06_V2d *size) {
    reinterpret_cast<void (__cdecl*)(int, S06_V2d *)>(0x004c1c80u)(font, size);
}
#endif
}
