// MATCH-S06: port body of 0x004295a0 = the byte-matched source re/match/s06/004295a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00427e00 -> FUN_00427e00 (symmap new-body-batch2 sw4)
//   0x0040dc80 -> UtilFloat63b910Get in the .asi (Util/PromoLoop_round9.cpp, .asi-only TU); the exe makes the raw-address call
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004295a0 HudDualLabelRender
#define FUN_00427e00 FUN_00427e00
#ifdef MASHED_STANDALONE
#define FUN_0040dc80 MatchS06_004295a0_Call0040dc80
#else
#define FUN_0040dc80 UtilFloat63b910Get
#endif
#pragma comment(linker, "/EXPORT:HudDualLabelRender=_HudDualLabelRender")
extern "C" {
#include "../../../../re/match/s06/004295a0.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
#ifdef MASHED_STANDALONE
float MatchS06_004295a0_Call0040dc80(void) {
    return reinterpret_cast<float (__cdecl*)()>(0x0040dc80u)();
}
#endif
}
RH_ScopedInstall(HudDualLabelRender, 0x004295a0);
