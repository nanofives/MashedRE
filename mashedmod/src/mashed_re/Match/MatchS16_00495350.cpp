// MATCH-S16: port body of 0x00495350 = the byte-matched source re/match/s16/00495350.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


#define MATCH_PORT 1
#define FUN_00495350 HardwareShowIntroVideo
#define FUN_004671a0 MatchS16_00495350_Call004671a0
#define FUN_00492d20 IntroSplashFrameTickShim
#define FUN_00493f70 VideoStateFlagGet
#define FUN_00493f80 IntroVideoDimGetter
#define FUN_00493fc0 AspectRatioGlobalGet
#define FUN_00493fd0 MatchS16_00495350_Call00493fd0
#define FUN_00494460 MatchS16_00495350_Call00494460
#define FUN_00494480 MatchS16_00495350_Call00494480
#define FUN_00499710 MatchS16_00495350_Call00499710
#define FUN_004c19f0 RwVtableSlot07Call
#define FUN_004c1a00 IntroSplashVtableSlot6
#define FUN_004c1bb0 IntroSplashRenderState
#define FUN_004c1be0 MatchS16_00495350_Call004c1be0
#pragma comment(linker, "/EXPORT:HardwareShowIntroVideo=_HardwareShowIntroVideo")
extern "C" {
#include "../../../../re/match/s16/00495350.c"
}
// Callees the old Frontend/IntroSplash.cpp body reached through their original addresses (its static Orig* call-throughs,
// no C-callable port body): raw-address forwarders, the same calls. Callees with a C-linkage port body bind to it above.
extern "C" {
int MatchS16_00495350_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
void MatchS16_00495350_Call00493fd0(int cam, V2 *p1, V2 *p2, int p3, int p4) {
    reinterpret_cast<void (__cdecl*)(int, V2 *, V2 *, int, int)>(0x00493fd0u)(cam, p1, p2, p3, p4);
}
void MatchS16_00495350_Call00494460(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00494460u)(p0);
}
int MatchS16_00495350_Call00494480(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x00494480u)(p0);
}
int MatchS16_00495350_Call00499710(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x00499710u)(p0);
}
void MatchS16_00495350_Call004c1be0(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int, int)>(0x004c1be0u)(p0, p1);
}
}
