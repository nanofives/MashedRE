// ATOMIC-VIEW (L-111 step 1): exe-only port body of rva 0x004672d0 = the matched source re/match/s11/004672d0.c
// (BYTE-IDENTICAL 45 B, relocheck OK): when 0x0042b930() == 3 and param != -1, *(0x0042f510() + 4), else
// *(DAT_006905b0 + 4). Caller on this path: the matched 0x0040cda0 with param 0. DAT_006905b0 is the camera record
// the matched 0x00467110 builds (from 0x004921d0, before AppInitialiseOnBootup's 0x0045bae0 loop); +0x04 is its librw
// frame. The 0x0040cda0 forwarder checks the record before the call.
// Callees bind by name: 0x0042b930 = MenuAlphaGet (Match/MatchS07_0042b930.cpp), 0x0042f510 = Vehicle0HandleGet
// (Match/MatchS07_0042f510.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004672d0 Orig_004672d0
#define FUN_0042b930 MenuAlphaGet
#define FUN_0042f510 Vehicle0HandleGet
extern "C" {
#include "../../../../re/match/s11/004672d0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
