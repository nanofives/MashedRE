// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane): exe-only port body of rva 0x0042c960 = the matched source re/match/s07/0042c960.c
// (match.py 100%, hooks.csv name CameraTransitionStateMachine). Its one caller is 0x0043c000 (call site 0x0043c2d9),
// Match/PortTop_0043c000.cpp. Callees bind by name; the two with no exe body are the named stand-ins of
// Match/PortTop_StandIns08.cpp (0x0042c510) and Match/PortTop_StandIns05.cpp (0x004111c0, the race tick the knob path
// does not run: the hand session runs the race).
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042c960 Orig_0042c960
#define FUN_004111c0 PortTop_StandIn05_004111c0   // stand-in: counted, not run (PORT-TOP-05)
#define FUN_00429840 RaceStateLatchSet            // matched body, Match/MatchS06_00429840.cpp (exe since PORT-TOP-08)
#define FUN_0042c510 PortTop_StandIn08_0042c510   // stand-in: re/match/s07/0042c510.c 84.2%, equiv.py NOT-PROVEN
#define FUN_00429870 LapTimeALessThanB            // matched exe body, Match/MatchS06_00429870.cpp
#define FUN_00424270 Orig_00424270                // admitted body (ADMITTED.tsv, EQUIV), Match/PortTop_00424270.cpp
#define FUN_0040e340 GetLiveCarCount              // matched exe body, Match/MatchS02_0040e340.cpp
// FUN_0042c7c0 keeps its name: matched body Match/MatchS07_0042c7c0.cpp (exe since PORT-TOP-08)
#define FUN_004282a0 MenuMenusBA                  // matched exe body, Match/MatchS06_004282a0.cpp
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
#define FUN_00424100 TeamBlockZeroGet             // matched exe body, Match/MatchS05_00424100.cpp
#define FUN_004241c0 TeamBlockOneGet              // matched body, Match/MatchS05_004241c0.cpp (exe since PORT-TOP-08)
// FUN_0042c300 keeps its name: matched body Match/MatchS07_0042c300.cpp (exe since PORT-TOP-08)
#define FUN_0042c3c0 Orig_0042c3c0                // matched exe body, Match/PortTop_0042c3c0.cpp
#define FUN_0040b460 Orig_0040b460                // matched exe body, Match/PortTop_0040b460.cpp
#define FUN_0040b540 PlayerScoreRankOrder         // matched exe body, Match/MatchS02_0040b540.cpp
#define FUN_0040b6b0 ModeScoreGetBySlot           // matched exe body, Match/MatchS02_0040b6b0.cpp
#define FUN_0040b6c0 FrontendArrayGet             // matched exe body, Match/MatchS02_0040b6c0.cpp
extern "C" {
#include "../../../../re/match/s07/0042c960.c"
}
#endif // MASHED_STANDALONE
