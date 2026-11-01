// MATCH-S06: port body of 0x00429aa0 = the byte-matched source re/match/s06/00429aa0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/UtilMid.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Callees:
//   0x00430790 -> GetDat0067f17c (Util/SmallLeaves_o6.cpp)
//   0x00430820 -> Global67ed6cGet in the .asi (Util/PromoLoop_round27.cpp, .asi-only TU); the exe makes the raw-address call
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00429aa0 GameStateSlotsFill
#define FUN_00430790 GetDat0067f17c
#ifdef MASHED_STANDALONE
#define FUN_00430820 MatchS06_00429aa0_Call00430820
#else
#define FUN_00430820 Global67ed6cGet
#endif
#pragma comment(linker, "/EXPORT:GameStateSlotsFill=_GameStateSlotsFill")
extern "C" {
#include "../../../../re/match/s06/00429aa0.c"
}

// Raw-address forwarders (call-throughs into the original at the fixed address, not bodies of the callees).
extern "C" {
#ifdef MASHED_STANDALONE
int MatchS06_00429aa0_Call00430820(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00430820u)();
}
#endif
}
RH_ScopedInstall(GameStateSlotsFill, 0x00429aa0);
