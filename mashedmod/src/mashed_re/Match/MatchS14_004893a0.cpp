// MATCH-S14: port body of 0x004893a0 = the byte-matched source re/match/s14/004893a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004893a0 AdjThunk4893a0
#define FUN_00476cb0 Batch476cb0
// FUN_00476cb0 -> Batch476cb0 (Render/RfxEmitter.cpp, installed at 0x00476cb0 in the .asi). The old naked body
// tail-jumped to the raw address 0x00476cb0, which the .asi redirects to that same port body.
#pragma comment(linker, "/EXPORT:AdjThunk4893a0=_AdjThunk4893a0")
extern "C" {
#include "../../../../re/match/s14/004893a0.c"
}
RH_ScopedInstall(AdjThunk4893a0, 0x004893a0);
