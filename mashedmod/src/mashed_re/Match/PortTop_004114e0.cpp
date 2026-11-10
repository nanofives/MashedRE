// PORT-OT-VEHICLE (L-111 (a) step 1): exe-only port body of rva 0x004114e0 = the matched source re/match/s02/004114e0.c
// (MATCH-S02, BYTE-IDENTICAL): for each nonzero DAT_0063bb04 / DAT_0063bb08, store FUN_00483a40(it) (0) back, then
// DAT_0063bb14 = DAT_0063bb10 = DAT_0063bb2c = 0. The matched 0x0040cfd0 (Match/PortTop_0040cfd0.cpp) calls it on the
// MASHED_ORIG_TOP path (WinMain 0x00492370 -> AppDestroy 0x00402a40 -> 0x0040cfd0).
// Callee binds by name, no raw-address call: 0x00483a40 Orig_00483a40 (Match/PortTop_00483a40.cpp).
// Shared state: no exe TU writes 0x0063bb04 / 0x0063bb08 (their matched writer, 0x00411d90 re/match/topm01/00411d90.c:117-134,
// is not compiled into the exe), so both stay 0 and the free branch does not run. 0x0063bb10 / 0x0063bb14 are read-only in
// the exe (Vehicle/GhostSetupRender.cpp, Vehicle/SmallLeaves_q4.cpp, Match/MatchS02_00411d60.cpp), and 0x0063bb2c has no
// exe reader. The three zero stores are the ones the original makes from AppDestroy, on the same addresses.
// The other body of this RVA is the .asi-only Vehicle/Replay.cpp ReplayCleanup, so this TU is the ORIGTOP-ONLY class
// (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004114e0 Orig_004114e0
#define FUN_00483a40 Orig_00483a40                              // matched, Match/PortTop_00483a40.cpp
extern "C" {
#include "../../../../re/match/s02/004114e0.c"
}
#endif  // MASHED_STANDALONE
