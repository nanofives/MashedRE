// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00485d90 = the matched source
// re/match/s14/00485d90.c (BYTE-IDENTICAL): for the four 0x23c-byte records at 0x006fde10 (i = 3..0), pool =
// 0x004892c0(0x0040bb30("exp_cloud2"), 0x23, 8, 0x37), 0x004893a0(pool, 5, 2), index = i, and the five 7-entry arrays and
// +0x238 set to 0. 0x005ce5f8 is seeded (OT_MEASURE.tsv). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it
// after the matched 0x00489290, which marks the 100 pools free (m_58 = 1), so 0x004892c0 returns a pool for each record.
// Its teardown 0x00485e10 (Match/PortTop_00485e10.cpp, from the matched 0x0040bd00) frees the four pools.
// Callees bind by name, no raw-address call: 0x0040bb30 -> Search40bb30 (Match/MatchS02_0040bb30.cpp), 0x004892c0 ->
// Orig_004892c0, 0x004893a0 -> Orig_004893a0 (exe-only, Match/PortTop_004892c0.cpp / Match/PortTop_004893a0.cpp).
// Shared state: no exe TU reads or writes 0x006fde10..0x006fe6ff.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00485d90 Orig_00485d90
#define FUN_0040bb30 Search40bb30                               // matched, Match/MatchS02_0040bb30.cpp
#define FUN_004892c0 Orig_004892c0                              // matched, Match/PortTop_004892c0.cpp
#define FUN_004893a0 Orig_004893a0                              // matched, Match/PortTop_004893a0.cpp
extern "C" {
#include "../../../../re/match/s14/00485d90.c"
}
#endif  // MASHED_STANDALONE
