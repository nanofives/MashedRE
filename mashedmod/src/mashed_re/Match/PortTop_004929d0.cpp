// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05 (L-96 top-down lane): exe-only port body of rva 0x004929d0 = the matched source re/match/s15/004929d0.c
// (match.py 100%), the game-mode machine on DAT_00771968 the run function 0x00492290 calls first in each frame.
// Reached only on the MASHED_ORIG_TOP path (PortTop_Bound_004929d0). The default path's copy is the 95.7% source
// re/match/gffu/004929d0.c in Race/GameModeMachine.cpp (MASHED_PORTSTATE_GAMEFLOW_FU, default OFF); this TU uses the
// 100% source. Callees bind by name to their exe bodies (the same table as Race/GameModeMachine.cpp, without its
// logging forwarders), and case 2's 0x0040d440 to the matched Orig_0040d440 (Match/PortTop_0040d440.cpp).
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004929d0 Orig_004929d0
#define FUN_004927c0 Step771980Advance        // Match/MatchGFFU_004927c0.cpp
#define FUN_0042c2e0 GetDat0067ecb8           // Match/MatchS07_0042c2e0.cpp
#define FUN_0042c2d0 GetDat0067ecb4           // Match/MatchS07_0042c2d0.cpp
#define FUN_0042b940 StatePhaseSubSet         // Match/MatchS07_0042b940.cpp
#define FUN_0042c2f0 SetDat0067ecb8           // Match/MatchS07_0042c2f0.cpp
#define FUN_0042b930 MenuAlphaGet             // Match/MatchS07_0042b930.cpp
#define FUN_0042b8d0 StatePhaseIsIdle         // Match/MatchS07_0042b8d0.cpp
#define FUN_0040d440 Orig_0040d440            // Match/PortTop_0040d440.cpp (PORT-TOP-05)
#define FUN_0042c1d0 RaceStateArrayZero       // Match/MatchS07_0042c1d0.cpp
#define FUN_0042c220 FUN_0042c220             // Match/MatchS07_0042c220.cpp
#define FUN_0042c280 Init42c280               // Match/MatchS07_0042c280.cpp
#define FUN_0042c1c0 RaceInterruptFlagGet     // Match/MatchS07_0042c1c0.cpp
#define FUN_0042f6a0 GetRaceSubMode           // Match/MatchS07_0042f6a0.cpp
#define FUN_0040e350 GetRenderSubMode         // Match/MatchS02_0040e350.cpp
#define FUN_005c9d00 GetRaceEndTrigger        // Util/GameStateGetters.cpp
#define FUN_00432080 RaceEndCheckFinish       // Match/MatchS07_00432080.cpp
#define FUN_0040e450 PortTop_Fwd05_0040e450   // Flag63b908Get (Match/MatchS02_0040e450.cpp) returns unsigned int
#define FUN_0042b910 RaceEndConstGet          // Match/MatchS07_0042b910.cpp
#define FUN_004331a0 RaceFinalizeOnce         // Match/MatchS08_004331a0.cpp
#define FUN_0040b6e0 Add40b6e0                // Match/MatchS02_0040b6e0.cpp
#define FUN_0040b540 PlayerScoreRankOrder     // Match/MatchS02_0040b540.cpp
#define FUN_0040e340 GetLiveCarCount          // Match/MatchS02_0040e340.cpp
#define FUN_0042f500 GetDat0067ea64           // Match/MatchS07_0042f500.cpp
#define FUN_0040b430 UtilFieldAdd8a94f0       // Match/MatchS02_0040b430.cpp
#define FUN_0040b700 Counters8a9530Inc        // Match/MatchS02_0040b700.cpp
#define FUN_0040b6d0 UtilArrayGet8a94e0       // Match/MatchS02_0040b6d0.cpp
#define FUN_00448700 VehicleSeedWritePair     // Match/MatchS08_00448700.cpp
#define FUN_0040e460 Flag63b908Set            // Match/MatchS02_0040e460.cpp
#define FUN_0042b8f0 StatePhaseIsFinal        // Match/MatchS07_0042b8f0.cpp
#define FUN_0042b8e0 StatePhaseIsTwo          // Match/MatchS07_0042b8e0.cpp
extern "C" unsigned int __cdecl Flag63b908Get(void);
extern "C" int PortTop_Fwd05_0040e450(void) { return static_cast<int>(Flag63b908Get()); }
extern "C" {
#include "../../../../re/match/s15/004929d0.c"
}
#endif // MASHED_STANDALONE
