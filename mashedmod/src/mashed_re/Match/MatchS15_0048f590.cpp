// MATCH-S15: port body of 0x0048f590 = the byte-matched source re/match/s15/0048f590.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0048f590 ParticlePoolAlloc48f590
#pragma comment(linker, "/EXPORT:ParticlePoolAlloc48f590=_ParticlePoolAlloc48f590")
extern "C" {
#include "../../../../re/match/s15/0048f590.c"
}
RH_ScopedInstall(ParticlePoolAlloc48f590, 0x0048f590);
