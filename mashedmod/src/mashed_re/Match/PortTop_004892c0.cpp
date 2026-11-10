// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x004892c0 = the matched source
// re/match/s14/004892c0.c (BYTE-IDENTICAL): takes the first of the 100 pools at 0x00703118 with m_58 != 0 (returns 0 if
// none), and for param_2 != 0 and param_3 != 0 fills it: m_max, m_5c = DAT_005cc320 / param_3, m_50 = flags (0x817 for 0),
// 0x004770c0(pool+0x10, flags, param_2 * param_3, tex), 0x00476cb0(pool+0x10, 5, 6), m_54 = 1, m_58 = 0, m_items =
// 0x00474db0(param_2, 0x30) with each item's position 0 and colour bytes 0xff. 0x005cc320 is seeded (OT_MEASURE.tsv).
// Its only caller on the MASHED_ORIG_TOP path is the matched 0x00485d90 (Match/PortTop_00485d90.cpp).
// Callees bind by name, no raw-address call: 0x004770c0 -> Orig_004770c0, 0x00476cb0 -> Batch476cb0, 0x00474db0 ->
// Orig_00474db0 (Match/PortTop_00474db0.cpp: RwGlobals +0x108 librw malloc + zero fill).
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004892c0 Orig_004892c0
#define FUN_004770c0 Orig_004770c0                              // matched, Match/PortTop_004770c0.cpp
#define FUN_00476cb0 Batch476cb0                                // matched, Match/MatchS12_00476cb0.cpp
#define FUN_00474db0 Orig_00474db0                              // matched, Match/PortTop_00474db0.cpp
extern "C" {
#include "../../../../re/match/s14/004892c0.c"
}
#endif  // MASHED_STANDALONE
