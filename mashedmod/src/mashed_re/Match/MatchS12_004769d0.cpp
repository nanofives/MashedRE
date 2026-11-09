// PORT-STATE-02 (2026-11-08, L-101 seed default ON): port body of 0x004769d0 = the byte-matched source re/match/s12/004769d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in both targets (moved out of Render/RfxEmitter.cpp). Compiled here as C++
// with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL). Its file-backed .data reads come from
// the original in the .asi and from the port-state seed (Compat/PortStateSeed.cpp) in the exe.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004769d0 ParticleEmitter_SetVelocity
#pragma comment(linker, "/EXPORT:ParticleEmitter_SetVelocity=_ParticleEmitter_SetVelocity")
extern "C" {
#include "../../../../re/match/s12/004769d0.c"
}
RH_ScopedInstall(ParticleEmitter_SetVelocity, 0x004769d0);
