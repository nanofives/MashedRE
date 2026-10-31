// MATCH-S04: port body of 0x0041db80 = the byte-matched source re/match/s04/0041db80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of HUD/HudBatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The file also holds the tail-jump target 0x0041dab0 (HIDDEN marker: no Ghidra entry, no hooks row) as a `static`
// callee; no port TU had a body for it (the old HudBatch.cpp body inlined it). No install for it.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0041db80 Sub0041db80_HudThresholdDispatch
#pragma comment(linker, "/EXPORT:Sub0041db80_HudThresholdDispatch=_Sub0041db80_HudThresholdDispatch")
extern "C" {
#include "../../../../re/match/s04/0041db80.c"
}
RH_ScopedInstall(Sub0041db80_HudThresholdDispatch, 0x0041db80);
