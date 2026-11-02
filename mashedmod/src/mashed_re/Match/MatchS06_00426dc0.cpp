// MATCH-S06: port body of 0x00426dc0 = the byte-matched source re/match/s06/00426dc0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/BatchAA_s1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callee 0x00479100: no port body; the old body called it through its original address.
// The matched function returns void, as the original's C does: EAX on return is FUN_00479100's value, which the
// original's sole caller tests (0x0041f630). The old port body returned it explicitly. Here it survives in EAX
// (call; add esp,0x10; ret), the same as the original's code.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00426dc0 FrontendRaycastForward
#define FUN_00479100 MatchS06_00426dc0_Call00479100
extern "C" unsigned int MatchS06_00426dc0_Call00479100(void * p0, float * p1, float * p2, float * p3) {
    return reinterpret_cast<unsigned int (__cdecl*)(void *, float *, float *, float *)>(0x00479100u)(p0, p1, p2, p3);
}
#pragma comment(linker, "/EXPORT:FrontendRaycastForward=_FrontendRaycastForward")
extern "C" {
#include "../../../../re/match/s06/00426dc0.c"
}
RH_ScopedInstall(FrontendRaycastForward, 0x00426dc0);
