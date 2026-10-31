// MATCH-S04: port body of 0x0041ccc0 and of its callee 0x0041c9a0 = the byte-matched source re/match/s04/0041c9a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100% for both MATCH lines).
// The same source text is these RVAs' only body in the targets that compile this TU (0x0041ccc0 moved out of
// HUD/HudDispatch.cpp; 0x0041c9a0 had no port body, the old 0x0041ccc0 called it through its original address).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The matched 0x0041c9a0 is `static` (the DDK passes its record in EAX) with no caller outside this file and no
// install, so it stays a cdecl static here.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041ccc0 HudSlotLoopCcc0
#pragma comment(linker, "/EXPORT:HudSlotLoopCcc0=_HudSlotLoopCcc0")
extern "C" {
#include "../../../../re/match/s04/0041c9a0.c"
}
RH_ScopedInstall(HudSlotLoopCcc0, 0x0041ccc0);
