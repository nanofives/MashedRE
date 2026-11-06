// MATCH-S14: port body of 0x00486610 = the byte-matched source re/match/s14/00486610.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Particle/ParticleBurst_ac1.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00486610 ParticleBurst4
#define FUN_004864f0 MatchS14_00486610_Call004864f0
// Callee 0x004864f0 has no port body: the old body called it through its original address; this forwarder makes
// the same call (param_1 passed as the same dword).
extern "C" void MatchS14_00486610_Call004864f0(unsigned int p0, unsigned int p1, int p2) {
    reinterpret_cast<void (__cdecl*)(unsigned int, unsigned int, int)>(0x004864f0u)(p0, p1, p2);
}
#pragma comment(linker, "/EXPORT:ParticleBurst4=_ParticleBurst4")
extern "C" {
#include "../../../../re/match/s14/00486610.c"
}
RH_ScopedInstall(ParticleBurst4, 0x00486610);
