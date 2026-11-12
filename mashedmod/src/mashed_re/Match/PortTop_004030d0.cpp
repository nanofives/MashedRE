// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x004030d0 = the matched source re/match/s01/004030d0.c
// (match.py 100%, relocheck OK; MGLOBAL form since PORT-TOP-05, byte-identical). Called by 0x00492d30 case 1 when
// 0x0042b930 returns 0x21. Reached only on the MASHED_ORIG_TOP path. Callees bind by name:
//   0x0042c1c0  RaceInterruptFlagGet       matched exe body, Match/MatchS07_0042c1c0.cpp
//   0x00472640  Set86ecc8                  matched exe body, Match/MatchS11_00472640.cpp (exe since PORT-TOP-05)
//   0x0042b950  Set7f1a0c_1000             matched exe body, Match/MatchS07_0042b950.cpp
//   0x0043df00  PortTop_StandIn05_0043df00 stand-in: matched (re/match/s08/0043df00.c), not compiled (level 2)
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004030d0 Orig_004030d0
#define FUN_0042c1c0 RaceInterruptFlagGet
#define FUN_00472640 Set86ecc8
#define FUN_0042b950 Set7f1a0c_1000
#define FUN_0043df00 PortTop_StandIn05_0043df00
extern "C" {
#include "../../../../re/match/s01/004030d0.c"
}
#endif // MASHED_STANDALONE
