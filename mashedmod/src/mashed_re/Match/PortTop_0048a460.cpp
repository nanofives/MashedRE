// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x0048a460 = the matched source
// re/match/s14/0048a460.c (BYTE-IDENTICAL): stores 0 to dwords +0 and +4 of each 0x330-byte record from 0x00709238 up to
// 0x00713198. The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it last on the MASHED_ORIG_TOP path.
// No callee. Shared state: no exe TU reads or writes 0x00709238..0x00713197 (the .asi-only Util/PromoLoop_round64.cpp is
// the only other source that names it).
// The other body of this RVA is the .asi-only Match/MatchS14_0048a460.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048a460 Orig_0048a460
extern "C" {
#include "../../../../re/match/s14/0048a460.c"
}
#endif  // MASHED_STANDALONE
