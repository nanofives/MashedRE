// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x005c4d30 = the matched source re/match/s28/005c4d30.c:
// returns param_1[0] when param_1[1] != 0, else 0. Its only caller in the exe is the matched 0x00555830
// (Match/PortTop_00555830.cpp, MASHED_ORIG_TOP path with MASHED_ORIG_TOP_RWMOD=1, from the matched 0x00427620). No callees.
// The other body of this RVA is the .asi-only Util/PromoLoop_sessionB.cpp CondGet5c4d30, so this TU is the ORIGTOP-ONLY
// class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005c4d30 Orig_005c4d30
extern "C" {
#include "../../../../re/match/s28/005c4d30.c"
}
#endif  // MASHED_STANDALONE
