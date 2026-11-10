// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x004893a0 = the matched source
// re/match/s14/004893a0.c (BYTE-IDENTICAL): 0x00476cb0(param_1 + 4 dwords, param_2, param_3). Its only caller on the
// MASHED_ORIG_TOP path is the matched 0x00485d90 (Match/PortTop_00485d90.cpp).
// Callee binds by name, no raw-address call: 0x00476cb0 -> Batch476cb0 (Match/MatchS12_00476cb0.cpp).
// The other body of this RVA is the .asi-only Match/MatchS14_004893a0.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004893a0 Orig_004893a0
#define FUN_00476cb0 Batch476cb0                                // matched, Match/MatchS12_00476cb0.cpp
extern "C" {
#include "../../../../re/match/s14/004893a0.c"
}
#endif  // MASHED_STANDALONE
