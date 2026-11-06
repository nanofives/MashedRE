// MATCH-S15: port body of 0x00492d20 = the byte-matched source re/match/s15/00492d20.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

// Raw-address forwarder: the old body called the original FUN_004967e0 through its address (no port body).
extern "C" void MatchS15_00492d20_Call004967e0(void) {
    typedef void (__cdecl* Fn)(void);
    reinterpret_cast<Fn>(0x004967e0u)();
}

#define MATCH_PORT 1
#define FUN_00492d20 IntroSplashFrameTickShim
#define FUN_004967e0 MatchS15_00492d20_Call004967e0
#pragma comment(linker, "/EXPORT:IntroSplashFrameTickShim=_IntroSplashFrameTickShim")
extern "C" {
#include "../../../../re/match/s15/00492d20.c"
}
RH_ScopedInstall(IntroSplashFrameTickShim, 0x00492d20);
