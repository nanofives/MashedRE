// MATCH-S12: port body of 0x004769a0 = the byte-matched source re/match/s12/004769a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RfxEmitter.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004769a0 ParticleEmitter_SetPosition
#pragma comment(linker, "/EXPORT:ParticleEmitter_SetPosition=_ParticleEmitter_SetPosition")
extern "C" {
#include "../../../../re/match/s12/004769a0.c"
}
RH_ScopedInstall(ParticleEmitter_SetPosition, 0x004769a0);
