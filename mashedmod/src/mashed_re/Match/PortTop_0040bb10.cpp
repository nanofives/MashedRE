// ATOMIC-VIEW (L-111 step 1): exe-only port body of rva 0x0040bb10 = the matched source re/match/s29/0040bb10.c
// (MATCH-40BB10, BYTE-IDENTICAL 23 B, C4): 0x0040bae0(atomic, data), returns the atomic. Caller: the
// MashedRw_ClumpForAllAtomicsAsViews_004e66d0 binding on behalf of the matched 0x0040cda0, so the atomic is an RW-layout
// view. Callee: 0x0040bae0 = Orig_0040bae0 (Match/PortTop_0040bae0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040bb10 Orig_0040bb10
#define FUN_0040bae0 Orig_0040bae0
extern "C" {
#include "../../../../re/match/s29/0040bb10.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
