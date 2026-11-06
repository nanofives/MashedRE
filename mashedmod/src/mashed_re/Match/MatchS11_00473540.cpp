// MATCH-S11: port body of 0x00473540 = the byte-matched source re/match/s11/00473540.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00473540 GradientQuadHorizAlpha
#define FUN_0042b8b0 MatchS11_00473540_Call0042b8b0
#define FUN_0042b8c0 MatchS11_00473540_Call0042b8c0
// Callees: the old Frontend/MenuLeaves_af1.cpp body called the screen-dim getters through their original addresses
// (0x0042b8b0 / 0x0042b8c0; in mashed_re.exe those addresses carry exe_main.cpp's StandaloneThunks to
// Standalone_ScreenWidth / Standalone_ScreenHeight), so each forwarder makes that same call.
extern "C" short MatchS11_00473540_Call0042b8b0(void) {
    return reinterpret_cast<short (__cdecl*)()>(0x0042b8b0u)();
}
extern "C" short MatchS11_00473540_Call0042b8c0(void) {
    return reinterpret_cast<short (__cdecl*)()>(0x0042b8c0u)();
}
#pragma comment(linker, "/EXPORT:GradientQuadHorizAlpha=_GradientQuadHorizAlpha")
extern "C" {
#include "../../../../re/match/s11/00473540.c"
}
RH_ScopedInstall(GradientQuadHorizAlpha, 0x00473540);
