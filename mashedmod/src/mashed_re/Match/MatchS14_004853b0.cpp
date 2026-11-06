// MATCH-S14: port body of 0x004853b0 = the byte-matched source re/match/s14/004853b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Physics/SmplFzxStateBlock.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The exported entry SmplFzxStateBlockGetLogged and its shadow A/B moved here with the body unchanged; the entry only
// forwards to the matched function (SmplFzxStateBlockGetLoggedBody), it is not a second body.
// The format string is the matched source's literal (same text as .rdata 0x005cf310, which the old body passed).
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include <cstdint>

#define MATCH_PORT 1
#define FUN_004853b0 SmplFzxStateBlockGetLoggedBody
#define FUN_00485340 MatchS14_004853b0_Call00485340
#define FUN_004987b0 MatchS14_004853b0_Call004987b0
// Callees: the old body called both through their original addresses (no port body in the exe; 0x00485340's
// Get485340 is .asi-only in Util/PromoLoop_sessionB.cpp). These forwarders make the same raw-address calls.
extern "C" unsigned int MatchS14_004853b0_Call00485340(unsigned int p0) {
    return reinterpret_cast<unsigned int (__cdecl*)(unsigned int)>(0x00485340u)(p0);
}
// Variadic debug printf: a tail jump keeps the caller's stack (format + varargs) as is.
extern "C" __declspec(naked) void MatchS14_004853b0_Call004987b0(const char * /*p0*/, ...) {
    __asm {
        push 0x004987b0
        ret
    }
}
extern "C" {
#include "../../../../re/match/s14/004853b0.c"
}

// --- 0x004853b0 shadow A/B (TT-11), moved with the body from Physics/SmplFzxStateBlock.cpp ---------------------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
extern "C" __declspec(dllexport) std::uint32_t __cdecl SmplFzxStateBlockGetLogged(int id) {
    SHADOW_AB_COUNTER(ab, "SmplFzxStateBlockGetLogged", 0x004853b0u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, SmplFzxStateBlockGetLoggedBody, id);
}
RH_ScopedInstall(SmplFzxStateBlockGetLogged, 0x004853b0);
