// PORT-OT-FRONTEND (L-111 (a) step 1): exe-only port body of rva 0x00428390 FrontendStateSet = the matched source
// re/match/s06/00428390.c (MATCH-S06, match.py 100%): DAT_0067d960 = param_1. The matched AppInitialiseOnBootup 0x00402750
// (Match/PortTop_00402750.cpp) calls it with 3 at re/match/s01/00402750.c:189, through hand piece 5
// (Match/PortTop_00402750_Pieces.cpp PortTop_Piece_00428390), which runs this body first and then the hand frontend
// asset init. Reached only on the MASHED_ORIG_TOP path. The other body of this RVA is the .asi-only
// Match/MatchS06_00428390.cpp. In the exe, 0x0067d960 is read only by the matched 0x00428a30 / 0x00429290 sources, and
// neither runs on this path (0x00429290 is stored as a callback by 0x004b6560 and never called).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00428390 Orig_00428390
extern "C" {
#include "../../../../re/match/s06/00428390.c"
}
#endif  // MASHED_STANDALONE
