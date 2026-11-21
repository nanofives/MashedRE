// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00410d10 = re/match/s02/00410d10.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00410d10 PortTop_Bound_00410d10
#define FUN_00405890 Pred405890   // exe body (Match/MatchS01.cpp)
#define FUN_00408a50 PerCarRaceProgressGet   // exe body (Match/MatchS01.cpp)
#define FUN_00408a70 PortTop_Bound_00408a70
#define FUN_00408ad0 RaceScoreFloatGetBySlot   // exe body (Match/MatchS01.cpp)
#define FUN_0040e180 PortTop_RFStop_0040e180   // named stop
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_0040eee0 PortTop_RFStop_0040eee0   // named stop
#define FUN_00417730 VehicleRaceAngleGet   // exe body (Vehicle/VehicleState.cpp)
#define FUN_00422fd0 PortTop_Bound_00422fd0
#define FUN_00423b20 PortTop_Bound_00423b20
#define FUN_00442df0 PortTop_Bound_00442df0
#define FUN_00443080 PortTop_Bound_00443080
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_0046cbb0 PortTop_RFStop_0046cbb0   // named stop
#define FUN_004922e0 PortTop_Bound_004922e0
extern "C" {
#include "../../../../re/match/s02/00410d10.c"
}
#endif // MASHED_STANDALONE
