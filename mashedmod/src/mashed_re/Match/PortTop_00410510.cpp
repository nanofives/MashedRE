// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00410510 = re/match/pilot/a5_00410510.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00410510 PortTop_Bound_00410510
#define FUN_00405890 Pred405890   // exe body (Match/MatchS01.cpp)
#define FUN_0040b6d0 UtilArrayGet8a94e0   // exe body (Match/MatchS02_0040b6d0.cpp)
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_00417730 VehicleRaceAngleGet   // exe body (Vehicle/VehicleState.cpp)
#define FUN_00417740 Float89a870ToInt   // exe body (Match/MatchGFFU_00417740.cpp)
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00448700 PortTop_Bound_00448700
#define FUN_0044df80 PortTop_Bound_0044df80
#define FUN_00458f80 PortTop_Bound_00458f80
#define FUN_0045bed0 PortTop_RFStop_0045bed0   // named stop
#define FUN_0045bf30 Orig_0045bf30   // exe body (Match/PortTop_0045bae0.cpp)
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_0046cbb0 PortTop_RFStop_0046cbb0   // named stop
extern "C" {
#include "../../../../re/match/pilot/a5_00410510.c"
}
#endif // MASHED_STANDALONE
