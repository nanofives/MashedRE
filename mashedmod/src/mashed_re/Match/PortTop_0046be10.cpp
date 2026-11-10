// PORT-OT-GAMEPLAY (L-111 (a) step 1): exe-only port body of rva 0x0046be10 Write46be10 = the matched source
// re/match/s11/0046be10.c (MATCH-S11 h3, match.py 100%, 55 B): returns 0 when param_2 >= 0x10, else stores *param_1 at
// DAT_008815a0[param_2] +0xbfc + 4*param_3 and param_4 at +0xc7c + 4*param_3 (record stride 0xd04) and returns 1. No
// callees. Its only caller in the exe is the matched 0x004725f0 (Match/PortTop_004725f0.cpp) on the MASHED_ORIG_TOP path.
// The other body of 0x0046be10 is the .asi-only Write46be10 in Util/PromoLoop_sessionB.cpp.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0046be10 Orig_0046be10
extern "C" {
#include "../../../../re/match/s11/0046be10.c"
}
#endif  // MASHED_STANDALONE
