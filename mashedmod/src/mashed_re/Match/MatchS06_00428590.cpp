// MATCH-S06: port body of 0x00428590 = the byte-matched source re/match/s06/00428590.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/SubsystemInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x004c19f0 -> RwVtableSlot07Call (Boot/FrameDispatch.cpp)
//   0x00428450 -> HudSpinCoinAnim in the .asi (HUD/TextCluster.cpp, .asi-only TU); exe: missing stub (see below)
//   0x004c1a00 -> IntroSplashVtableSlot6 (Match/MatchS19_004c1a00.cpp, both targets)
//   0x004c1bb0 -> IntroSplashRenderState in the .asi; exe: RW binding (see below)
//   0x004671a0 -> raw-address forwarder in the .asi; exe: BackBuffRasterOpen
//   0x004c1be0 -> raw-address forwarder in the .asi; exe: Orig_004c1be0

#define MATCH_PORT 1
#define FUN_00428590 ViewportInit
#define FUN_004c19f0 RwVtableSlot07Call
// [PORT-TOP-04B] exe: the callees bind by name (MASHED_ORIG_TOP path, SubsystemInit 0x00492270). The .asi keeps its
// hooks / raw-address calls.
//   rva 0x004671a0 -> BackBuffRasterOpen (matched, Match/MatchS11_004671a0.cpp, exe since PORT-TOP-04B)
//   rva 0x004c1bb0 -> RW binding RwCameraClear (LibRw/RwApiBind.cpp): the matched body calls RwGlobals +0x9c
//   rva 0x004c1a00 -> IntroSplashVtableSlot6 (matched, Match/MatchS19_004c1a00.cpp, exe since PORT-TOP-04B): calls the
//                     camera record's +0x18, the RwApiBind begin-update binding
//   rva 0x00428450 -> missing stub PortTop_Missing_00428450 (Match/PortTop_00493710.cpp): its only body is the .asi-only
//                     naked-asm HudSpinCoinAnim (HUD/TextCluster.cpp) and its source is 84.6%, so an exe copy would be a
//                     second body (rva-lint CROSS-TARGET)
//   rva 0x004c1be0 -> Orig_004c1be0 (matched, Match/PortTop_004c1be0.cpp) -> RW binding RwRasterShowRaster
#ifdef MASHED_STANDALONE
#define FUN_00428450 PortTop_Missing_00428450
#define FUN_004c1a00 IntroSplashVtableSlot6
#define FUN_004c1bb0 MashedRw_CameraClear_004c1bb0
#define FUN_004671a0 BackBuffRasterOpen
#define FUN_004c1be0 Orig_004c1be0
#else
#define FUN_00428450 HudSpinCoinAnim
#define FUN_004c1a00 IntroSplashVtableSlot6
#define FUN_004c1bb0 IntroSplashRenderState
#define FUN_004671a0 MatchS06_00428590_Call004671a0
#define FUN_004c1be0 MatchS06_00428590_Call004c1be0
#endif
#pragma comment(linker, "/EXPORT:ViewportInit=_ViewportInit")
extern "C" {
#include "../../../../re/match/s06/00428590.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees). .asi only.
#ifndef MASHED_STANDALONE
extern "C" {
int MatchS06_00428590_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
void MatchS06_00428590_Call004c1be0(int p0, int p1, int p2) {
    reinterpret_cast<void (__cdecl*)(int, int, int)>(0x004c1be0u)(p0, p1, p2);
}
}
#endif
