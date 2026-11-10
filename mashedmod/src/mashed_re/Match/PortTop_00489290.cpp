// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00489290 = the matched source
// re/match/s14/00489290.c (BYTE-IDENTICAL): for the 100 0x68-byte pools at 0x00703118, m_54 = 0 and m_58 = 1. The matched
// 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP path, before 0x00485d90, whose 0x004892c0 takes
// the first pool with m_58 != 0.
// No callee. Shared state: no exe TU reads or writes 0x00703118..0x00705a17 (the .asi-only Lane2/L2_00489890.cpp is the
// only other source that names the range).
// The other body of this RVA is the .asi-only Match/MatchS14_00489290.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00489290 Orig_00489290
extern "C" {
#include "../../../../re/match/s14/00489290.c"
}
#endif  // MASHED_STANDALONE
