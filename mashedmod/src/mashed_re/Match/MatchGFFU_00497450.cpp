// MATCH-GFFU: port body of 0x00497450 = the byte-matched source re/match/gffu/00497450.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_sessionB.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// MATCH-PORTSTATE-GAMEFLOW-FU: an exe body for a callee of the game-mode machine 0x004929d0.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00497450 Pred497450
// 0x00496900 SlotActiveThunk: E9 4B 0B 00 00 = jmp 0x00497450 (0x00496905 + 0xb4b). A jmp thunk has no C source
// form; its body here forwards to this TU's 0x00497450 body (same return value, the thunk's only effect).
extern "C" int __cdecl Pred497450(int param_1);
extern "C" __declspec(dllexport) unsigned int __cdecl SlotActiveThunk(int player_idx) {
    return static_cast<unsigned int>(Pred497450(player_idx));   // cited 0x00496900 -> 0x00497450
}
RH_ScopedInstall(SlotActiveThunk, 0x00496900);   // moved out of Util/PromoLoop_round24.cpp (called 0x00497450 by address)
#pragma comment(linker, "/EXPORT:Pred497450=_Pred497450")
extern "C" {
#include "../../../../re/match/gffu/00497450.c"
}
RH_ScopedInstall(Pred497450, 0x00497450);
