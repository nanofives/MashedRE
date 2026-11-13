// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x0043d7c0 = the matched source re/match/s08/0043d7c0.c
// (match.py 100%), the modal box step on DAT_0067eab0 that 0x00492d30 calls in cases 1, 4, 5, 6 and 7. Reached only on
// the MASHED_ORIG_TOP path. Callees bind by name; the ones with no exe body are the named stand-ins of
// Match/PortTop_StandIns05.cpp (each runs only after a modal box reached state 3).
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0043d7c0 Orig_0043d7c0
#define FUN_0040ab40 PortTop_StandIn05_0040ab40   // stand-in: matched (re/match/s02/0040ab40.c), not compiled (level 2)
#define FUN_0040ac80 PortTop_StandIn05_0040ac80   // stand-in: matched (re/match/s02/0040ac80.c), not compiled (level 2)
#define FUN_0040b810 TimerGlobalsReset            // matched exe body, Match/MatchS02_0040b810.cpp
#define FUN_0040de10 PortTop_StandIn05_0040de10   // stand-in: matched (re/match/s02/0040de10.c), not compiled (level 2)
#define FUN_0040e360 PortTop_StandIn05_0040e360   // stand-in: port body RaceModeSet is in the .asi-only multi-function Util/PromoLoop_round10.cpp
#define FUN_00413f90 TimerGetBasePtr              // matched exe body, Match/MatchS08FU_00413f90.cpp
#define FUN_00422b30 TimerArrayClear              // matched exe body, Match/MatchS05_00422b30.cpp
#define FUN_004298c0 Clear67d99c_x4               // matched exe body, Match/MatchS06_004298c0.cpp
#define FUN_00429aa0 GameStateSlotsFill           // matched exe body, Match/MatchS06_00429aa0.cpp
#define FUN_0042ae10 MenuReadinessCheckA          // matched exe body, Match/MatchS06_0042ae10.cpp
#define FUN_0042aeb0 MenuReadinessCheckB          // matched exe body, Match/MatchS07_0042aeb0.cpp
#define FUN_0042af50 MenuReadinessCheckC          // matched exe body, Match/MatchS07_0042af50.cpp
#define FUN_0042b950 Set7f1a0c_1000               // matched exe body, Match/MatchS07_0042b950.cpp
#define FUN_0042bf30 Post0042bf30                 // matched exe body, Match/MatchS07_0042bf30.cpp
#define FUN_0042c150 Scan42c150                   // matched exe body, Match/MatchS07_0042c150.cpp (exe since PORT-TOP-05)
#define FUN_0042c220 FUN_0042c220                 // matched exe body, Match/MatchS07_0042c220.cpp
#define FUN_0043d2a0 PortTop_StandIn05_0043d2a0   // stand-in: no matched source, no exe body (the hand screen stack is MenuNavSM)
#define FUN_0046dc00 EntityFieldSet               // matched exe body, Match/MatchS11_0046dc00.cpp
#define FUN_00493570 Set77196c_1                  // matched exe body, Match/MatchS15_00493570.cpp (exe since PORT-TOP-05)
#define FUN_00493580 Set771970_1                  // matched exe body, Match/MatchS15_00493580.cpp (exe since PORT-TOP-05)
extern "C" {
#include "../../../../re/match/s08/0043d7c0.c"
}
#endif // MASHED_STANDALONE
