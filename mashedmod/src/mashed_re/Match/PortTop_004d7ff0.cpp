// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x004d7ff0 = the matched source re/match/s21/004d7ff0.c
// (BYTE-IDENTICAL): returns param_1. The matched 0x005581f0 (Match/PortTop_005581f0.cpp) calls it on the MASHED_ORIG_TOP
// path (AppDestroy 0x00402a40), only on its param_1 == 0 branch (the RwError 0x180 / 0x80000016 path).
// No callee. No global.
// The other body of this RVA is the .asi-only Render/HighAB3Helpers_p6.cpp RwIdentityPassthrough, so this TU is the
// ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004d7ff0 Orig_004d7ff0
extern "C" {
#include "../../../../re/match/s21/004d7ff0.c"
}
#endif  // MASHED_STANDALONE
