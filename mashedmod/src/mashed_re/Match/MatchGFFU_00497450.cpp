// MATCH-GFFU: port body of 0x00497450 = the byte-matched source re/match/gffu/00497450.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00497450 Pred497450
// 0x00496900 SlotActiveThunk: E9 4B 0B 00 00 = jmp 0x00497450 (0x00496905 + 0xb4b).
// PORT-BATCH-FRONTEND (D4, L-107 (a), 2026-11-09): the hand forwarder that sat here is replaced by the byte-matched
// re/match/s16/00496900.c (100%, relocheck OK), compiled under the same exported name in both targets. Same prototype
// (one int argument, the 0x00497450 result in eax); the one exe caller, Match/MatchS07_0042c220.cpp, binds it by name.
#define FUN_00496900 SlotActiveThunk
#pragma comment(linker, "/EXPORT:Pred497450=_Pred497450")
#pragma comment(linker, "/EXPORT:SlotActiveThunk=_SlotActiveThunk")
extern "C" {
#include "../../../../re/match/gffu/00497450.c"
#include "../../../../re/match/s16/00496900.c"
}
RH_ScopedInstall(SlotActiveThunk, 0x00496900);   // moved out of Util/PromoLoop_round24.cpp, now the matched body
RH_ScopedInstall(Pred497450, 0x00497450);
