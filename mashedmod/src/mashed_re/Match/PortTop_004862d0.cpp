// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x004862d0 = the matched source
// re/match/s14/004862d0.c (BYTE-IDENTICAL): for the 150 0x7c-byte records at 0x006fe790, +0x70 / +0x74 / +0x78 = 0, then the
// record 0x006fe700 = 0x004770c0(0x427, 0xfa, "exp_cloud2") + 0x00476cb0(5, 2) + 0x00476be0(0) and 0x006fe740 =
// 0x004770c0(0x80c, 0x96, "scorch"). 0x005ce5f8 / 0x005ccca0 are seeded (OT_MEASURE.tsv). The matched 0x0040bbb0
// (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP path. Its teardown 0x00486350 (Match/PortTop_00486350.cpp,
// from the matched 0x0040bd00) runs 0x004768c0 on the two records.
// Callees bind by name, no raw-address call: 0x0040bb30 -> Search40bb30, 0x004770c0 -> Orig_004770c0 (flag 0x400 calls
// 0x00476be0 Batch476be0, flag 0x800 calls 0x00476c10 Batch476c10), 0x00476cb0 -> Batch476cb0, 0x00476be0 -> Batch476be0
// (Match/MatchS12_00476be0.cpp).
// Shared state: no exe TU reads or writes 0x006fe700..0x006fe78f or 0x006fe790..0x0070300f (the .asi-only
// Lane2/L2_00486f50.cpp is the only other source that names them).
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004862d0 Orig_004862d0
#define FUN_0040bb30 Search40bb30                               // matched, Match/MatchS02_0040bb30.cpp
#define FUN_004770c0 Orig_004770c0                              // matched, Match/PortTop_004770c0.cpp
#define FUN_00476cb0 Batch476cb0                                // matched, Match/MatchS12_00476cb0.cpp
#define FUN_00476be0 Batch476be0                                // matched, Match/MatchS12_00476be0.cpp
extern "C" {
#include "../../../../re/match/s14/004862d0.c"
}
#endif  // MASHED_STANDALONE
