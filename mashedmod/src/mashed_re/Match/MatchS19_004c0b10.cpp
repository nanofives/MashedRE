// MATCH-S19: port body of 0x004c0b10 = the byte-matched source re/match/s19/004c0b10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/RwAtomicDirtyFlag.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c0b10 RwAtomicDirtyFlagGet
#pragma comment(linker, "/EXPORT:RwAtomicDirtyFlagGet=_RwAtomicDirtyFlagGet")
extern "C" {
#include "../../../../re/match/s19/004c0b10.c"
}
RH_ScopedInstall(RwAtomicDirtyFlagGet, 0x004c0b10);
