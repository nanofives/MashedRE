// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004b68e0 = the matched source re/match/s18/004b68e0.c
// (return DAT_007d3e4c). In the original that word is the piz reader's open flag (0x004b6a14 stores 1). The exe's piz
// reader never writes 0x007d3e4c, so here the value is 0. Caller on the MASHED_ORIG_TOP path: the matched 0x004b65a0
// (Match/PortTop_004b65a0.cpp). The RVA also has an .asi-only body (Util/PromoLoop_round30.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b68e0 Orig_004b68e0
extern "C" {
#include "../../../../re/match/s18/004b68e0.c"
}
#endif  // MASHED_STANDALONE
