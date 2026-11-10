// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004b65a0 = the matched source re/match/s18/004b65a0.c
// (return FUN_004b68e0()). 0x004b68e0 is the exe-only matched Orig_004b68e0 (Match/PortTop_004b68e0.cpp). Callers on the
// MASHED_ORIG_TOP path: the matched builders 0x004587a0 and 0x00411f30 (both from 0x0045bae0), which discard the result.
// The RVA also has an .asi-only body (Util/PromoLoop_round30.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b65a0 Orig_004b65a0
#define FUN_004b68e0 Orig_004b68e0
extern "C" {
#include "../../../../re/match/s18/004b65a0.c"
}
#endif  // MASHED_STANDALONE
