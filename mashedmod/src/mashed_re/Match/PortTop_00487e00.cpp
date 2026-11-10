// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00487e00 and its static callee
// 0x00487150 = the matched source re/match/s14/00487150.c (both BYTE-IDENTICAL): 0x00487e00(n, tex) calls 0x00487150,
// which builds the 0x40-byte record at 0x00703068 with 0x004770c0(rec, 0x80c, n, tex) and 0x00476cb0(rec, 5, 6). The matched
// 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls 0x00487e00(0x32, 0x0040bb30("RWObjShad")) on the MASHED_ORIG_TOP path.
// Its teardown 0x00487140 (Match/PortTop_00487140.cpp, from the matched 0x0040bd00) runs 0x004768c0 on the same record.
// Callees bind by name, no raw-address call: 0x004770c0 -> Orig_004770c0 (Match/PortTop_004770c0.cpp), 0x00476cb0 ->
// Batch476cb0 (Match/MatchS12_00476cb0.cpp).
// Shared state: no exe TU reads or writes 0x00703050..0x007030a7 on this path. The exe-side mentions of the record are
// HUD/ScenarioWriters_sa2s1.cpp Particle3rdEmitterSubmit (the 0x00487d00 hook body, raw-address calls, reached only through
// an install) and comments in LibRw/RwRaceFx.cpp / .h.
// No other body of these RVAs in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00487e00 Orig_00487e00
#define FUN_00487150 Orig_00487150
#define FUN_004770c0 Orig_004770c0                              // matched, Match/PortTop_004770c0.cpp
#define FUN_00476cb0 Batch476cb0                                // matched, Match/MatchS12_00476cb0.cpp
extern "C" {
#include "../../../../re/match/s14/00487150.c"
}
#endif  // MASHED_STANDALONE
