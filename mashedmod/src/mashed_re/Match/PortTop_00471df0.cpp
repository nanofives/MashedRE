// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00471df0 = the matched source re/match/s11/00471df0.c
// (MATCH-S11 h6, 100%): clears the record table 0x006905c8, the name table 0x00690a30 (static callee 0x00471530) and the
// eight 0x2510 entries at 0x0086ece0. Called on the MASHED_ORIG_TOP path by the matched 0x00471eb0
// (Match/PortTop_00471eb0.cpp, from AppInitialiseOnBootup 0x00402750) and by the matched 0x0040cfd0
// (Match/PortTop_0040cfd0.cpp, from AppDestroy 0x00402a40). The source holds 0x00471530 as a file-static callee, whose
// other body is the .asi-only Match/MatchS11_00471530.cpp.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00471df0 Orig_00471df0
extern "C" {
#include "../../../../re/match/s11/00471df0.c"
}
#endif  // MASHED_STANDALONE
