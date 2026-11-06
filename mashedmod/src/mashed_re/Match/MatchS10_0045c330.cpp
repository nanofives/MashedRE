// MATCH-S10: port body of 0x0045c330 = the byte-matched source re/match/s10/0045c330.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Gameplay/PickupPredicates.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0045c330 PredNot45c330
// Callee FUN_0045bff0, per target, the same callee the old Gameplay/PickupPredicates.cpp body reached through its
// s_fn_45bff0 pointer: the exe binds the port's PK_SlotEmpty (Gameplay/PickupTick.cpp, exe-only, C linkage); the .asi
// calls the original address (there the installed JMP carries it into Util/PromoLoop_sessionB.cpp Pred45bff0).
#ifdef MASHED_STANDALONE
#define FUN_0045bff0 PK_SlotEmpty
#else
#define FUN_0045bff0 MatchS10_0045c330_Call0045bff0
extern "C" int MatchS10_0045c330_Call0045bff0(int slot) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x0045bff0u)(slot);
}
#endif
#pragma comment(linker, "/EXPORT:PredNot45c330=_PredNot45c330")
extern "C" {
#include "../../../../re/match/s10/0045c330.c"
}
RH_ScopedInstall(PredNot45c330, 0x0045c330);
