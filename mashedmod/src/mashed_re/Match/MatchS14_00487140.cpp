// MATCH-S14: port body of 0x00487140 = the byte-matched source re/match/s14/00487140.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/PromoLoop_round256.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00487140 Fwd4768c0_703068
#define FUN_004768c0 MatchS14_00487140_Call004768c0
// Callee 0x004768c0 has no port body: the old body called it through its original address. A tail jump, so the
// callee's EAX reaches 0x00487140's caller exactly as in the original (`push; call; pop ecx; ret` keeps EAX; the old
// body returned it as int).
extern "C" __declspec(naked) void MatchS14_00487140_Call004768c0(void * /*p0*/) {
    __asm {
        push 0x004768c0
        ret
    }
}
#pragma comment(linker, "/EXPORT:Fwd4768c0_703068=_Fwd4768c0_703068")
extern "C" {
#include "../../../../re/match/s14/00487140.c"
}
RH_ScopedInstall(Fwd4768c0_703068, 0x00487140);
