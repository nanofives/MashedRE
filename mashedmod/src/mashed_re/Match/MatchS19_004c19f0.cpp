// MATCH-S19: port body of 0x004c19f0 = the byte-matched source re/match/s19/004c19f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/FrameDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004c19f0 RwVtableSlot07Call
#pragma comment(linker, "/EXPORT:RwVtableSlot07Call=_RwVtableSlot07Call")
extern "C" {
#include "../../../../re/match/s19/004c19f0.c"
}
RH_ScopedInstall(RwVtableSlot07Call, 0x004c19f0);
