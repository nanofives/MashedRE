// MATCH-S06: port body of 0x00428a30 = the byte-matched source re/match/s06/00428a30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (new body, MATCH top-down rule L-72; no install).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00427c90 -> GetDat0067d84c (symmap swapped-batch1)
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x00428320 -> TextWidthMeasureB (symmap swap-batch2 sw4)
//   0x004288a0 -> FUN_004288a0 (symmap new-body-batch2 sw5)
//   0x004c19f0 -> RwVtableSlot07Call (Boot/FrameDispatch.cpp)
//   0x00428450 -> HudSpinCoinAnim in the .asi (HUD/TextCluster.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004950b0 -> QpcTimeScaledTo3Mhz in the .asi (Util/UtilMid.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1a00 -> IntroSplashVtableSlot6 in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004c1bb0 -> IntroSplashRenderState in the .asi (Frontend/IntroSplash.cpp, .asi-only TU); the exe makes the raw-address call
//   0x004671a0 -> raw-address forwarder: no port body (Frontend/IntroSplash.cpp OrigVehicle0Get is itself a raw call-through)
//   0x004a2b60 -> raw-address forwarder: no port body (original CRT formatter; variadic, so a naked tail-jump)
//   0x004c1be0 -> raw-address forwarder: no port body (IntroSplash.cpp OrigRenderTargetHelper is a raw call-through)

#define MATCH_PORT 1
#define FUN_00428a30 FUN_00428a30
#define FUN_00427c90 GetDat0067d84c
#define FUN_00427e00 FUN_00427e00
#define FUN_00428320 TextWidthMeasureB
#define FUN_004288a0 FUN_004288a0
#define FUN_004c19f0 RwVtableSlot07Call
#ifdef MASHED_STANDALONE
#define FUN_00428450 MatchS06_00428a30_Call00428450
#else
#define FUN_00428450 HudSpinCoinAnim
#endif
#ifdef MASHED_STANDALONE
#define FUN_004950b0 MatchS06_00428a30_Call004950b0
#else
#define FUN_004950b0 QpcTimeScaledTo3Mhz
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1a00 MatchS06_00428a30_Call004c1a00
#else
#define FUN_004c1a00 IntroSplashVtableSlot6
#endif
#ifdef MASHED_STANDALONE
#define FUN_004c1bb0 MatchS06_00428a30_Call004c1bb0
#else
#define FUN_004c1bb0 IntroSplashRenderState
#endif
#define FUN_004671a0 MatchS06_00428a30_Call004671a0
#define FUN_004a2b60 MatchS06_00428a30_Call004a2b60
#define FUN_004c1be0 MatchS06_00428a30_Call004c1be0
#pragma comment(linker, "/EXPORT:FUN_00428a30=_FUN_00428a30")
extern "C" {
#include "../../../../re/match/s06/00428a30.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
int MatchS06_00428a30_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
__declspec(naked) int MatchS06_00428a30_Call004a2b60(char *buf, const char *fmt, ...) {
    __asm { mov eax, 0x004a2b60  /* raw tail-jump, caller stack untouched */
             jmp eax }
}
void MatchS06_00428a30_Call004c1be0(int p0, int p1, int p2) {
    reinterpret_cast<void (__cdecl*)(int, int, int)>(0x004c1be0u)(p0, p1, p2);
}
#ifdef MASHED_STANDALONE
void MatchS06_00428a30_Call00428450(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x00428450u)(p0, p1);
}
unsigned int MatchS06_00428a30_Call004950b0(void) {
    return reinterpret_cast<unsigned int (__cdecl*)()>(0x004950b0u)();
}
int MatchS06_00428a30_Call004c1a00(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004c1a00u)(p0);
}
void MatchS06_00428a30_Call004c1bb0(int p0, MatchRGBA *p1, int p2) {
    reinterpret_cast<void (__cdecl*)(int, MatchRGBA *, int)>(0x004c1bb0u)(p0, p1, p2);
}
#endif
}
