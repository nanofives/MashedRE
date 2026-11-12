// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane, L-126): exe-only port body of rva 0x00424270 = re/match/s05/00424270.c, 99.6%,
// admitted by equiv.py (EQUIV [VALUE-FLOW], cross-check SAME 16/16 default and seeded; re/match/ADMITTED.tsv,
// verify/porttop08_20261112/RESULT_PORT-TOP-08.md). Admitted, not C4. Called by 0x0042c960 (Match/PortTop_0042c960.cpp).
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00424270 Orig_00424270
#define FUN_0040e340 GetLiveCarCount              // matched exe body, Match/MatchS02_0040e340.cpp
#define FUN_00423b40 PlayerScoreAccA              // matched body, Match/MatchS05_00423b40.cpp (exe since PORT-TOP-08)
#define FUN_00423b60 PlayerScoreAccB              // matched body, Match/MatchS05_00423b60.cpp (exe since PORT-TOP-08)
#define FUN_00423b80 PlayerScoreAccC              // matched body, Match/MatchS05_00423b80.cpp (exe since PORT-TOP-08)
#define FUN_00423ba0 PlayerScoreAccD              // matched body, Match/MatchS05_00423ba0.cpp (exe since PORT-TOP-08)
#define FUN_00423bc0 PlayerScoreTeamAccC          // matched body, Match/MatchS05_00423bc0.cpp (exe since PORT-TOP-08)
#define FUN_00423c40 PlayerScoreTeamAccBase       // matched body, Match/MatchS05_00423c40.cpp (exe since PORT-TOP-08)
#define FUN_00423cc0 PlayerScoreTeamAccGatedB     // matched exe body, Match/MatchS05_00423cc0.cpp
#define FUN_00423d50 PlayerScoreTeamAccBaseB      // matched exe body, Match/MatchS05_00423d50.cpp
#define FUN_00423dd0 PlayerScoreTeamAccGatedC     // matched exe body, Match/MatchS05_00423dd0.cpp
#define FUN_00423e60 PlayerScoreTeamAccCumB       // matched exe body, Match/MatchS05_00423e60.cpp
#define FUN_00423ee0 PlayerBlock2Field00Get       // matched exe body, Match/MatchS05_00423ee0.cpp
#define FUN_00423f60 PlayerBlock2Field04Get       // matched exe body, Match/MatchS05_00423f60.cpp
#define FUN_00423ff0 PlayerBlock2Field50Get       // matched exe body, Match/MatchS05_00423ff0.cpp
#define FUN_00424070 PlayerBlock2Field08Get       // matched exe body, Match/MatchS05_00424070.cpp
#define FUN_0042f500 GetDat0067ea64               // matched exe body, Match/MatchS07_0042f500.cpp
extern "C" {
#include "../../../../re/match/s05/00424270.c"
}
#endif // MASHED_STANDALONE
