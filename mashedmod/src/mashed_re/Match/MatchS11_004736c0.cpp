// MATCH-S11: port body of 0x004736c0 = the byte-matched source re/match/s11/004736c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuLeaves_af1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004736c0 BorderQuadFourAlpha
#define FUN_0042b8b0 MatchS11_004736c0_Call0042b8b0
#define FUN_0042b8c0 MatchS11_004736c0_Call0042b8c0
// Callees: the old Frontend/MenuLeaves_af1.cpp body called the screen-dim getters through their original addresses
// (0x0042b8b0 / 0x0042b8c0; in mashed_re.exe those addresses carry exe_main.cpp's StandaloneThunks to
// Standalone_ScreenWidth / Standalone_ScreenHeight), so each forwarder makes that same call.
extern "C" short MatchS11_004736c0_Call0042b8b0(void) {
    return reinterpret_cast<short (__cdecl*)()>(0x0042b8b0u)();
}
extern "C" short MatchS11_004736c0_Call0042b8c0(void) {
    return reinterpret_cast<short (__cdecl*)()>(0x0042b8c0u)();
}
#pragma comment(linker, "/EXPORT:BorderQuadFourAlpha=_BorderQuadFourAlpha")
extern "C" {
#include "../../../../re/match/s11/004736c0.c"
}
RH_ScopedInstall(BorderQuadFourAlpha, 0x004736c0);
