// PORT-OT-GAMEPLAY (L-111 (a) step 1): exe-only port body of rva 0x00405400 Clear639d70x3 = the matched source
// re/match/s01/004053d0.c (MATCH-S01, match.py 100%, 23 B BYTE-IDENTICAL): DAT_00639d74 = 0.0f, DAT_00639d78 = 0,
// DAT_00639d70 = 0. No callees. Called on the MASHED_ORIG_TOP path by the matched 0x0040cfd0 (Match/PortTop_0040cfd0.cpp,
// from AppDestroy 0x00402a40). The source also holds 0x004053d0, 0x00405420 and 0x00405430 on the same three globals.
// They compile here as Orig_004053d0 / Orig_00405420 / Orig_00405430 and nothing in the exe calls them. Each of the four
// has an .asi-only body (Render/LowRvaSetters_o2.cpp, Util/PromoLoop_round53.cpp, Render/PromoLoop_round11.cpp,
// Util/PromoLoop_sessionB.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004053d0 Orig_004053d0
#define FUN_00405400 Orig_00405400
#define FUN_00405420 Orig_00405420
#define FUN_00405430 Orig_00405430
extern "C" {
#include "../../../../re/match/s01/004053d0.c"
}
#endif  // MASHED_STANDALONE
