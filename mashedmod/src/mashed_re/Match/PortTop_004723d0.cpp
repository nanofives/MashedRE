// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x004723d0 Init691500 = the matched source
// re/match/s11/004723d0.c (MATCH-S11): ten 0x10-byte entries at 0x00691500..0x0069159f, words 0xff0Nffff / 0xff0N3cff
// then two zero dwords, N = 1..10. The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp) calls it on
// the MASHED_ORIG_TOP path. The other body of this RVA is the .asi-only Match/MatchS11_004723d0.cpp.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004723d0 Orig_004723d0
extern "C" {
#include "../../../../re/match/s11/004723d0.c"
}
#endif  // MASHED_STANDALONE
