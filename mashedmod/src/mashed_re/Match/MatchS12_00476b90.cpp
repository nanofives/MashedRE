// MATCH-S12: port body of 0x00476b90 = the byte-matched source re/match/s12/00476b90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RfxEmitter.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00476b90 Batch476b90
#pragma comment(linker, "/EXPORT:Batch476b90=_Batch476b90")
extern "C" {
#include "../../../../re/match/s12/00476b90.c"
}
RH_ScopedInstall(Batch476b90, 0x00476b90);
