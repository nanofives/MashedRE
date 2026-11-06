// MATCH-S12: port body of 0x00476a30 = the byte-matched source re/match/s12/00476a30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RfxEmitter.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00476a30 ParticleEmitter_SetScalar
#pragma comment(linker, "/EXPORT:ParticleEmitter_SetScalar=_ParticleEmitter_SetScalar")
extern "C" {
#include "../../../../re/match/s12/00476a30.c"
}
RH_ScopedInstall(ParticleEmitter_SetScalar, 0x00476a30);
