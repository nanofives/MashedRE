// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00429310 = re/match/s06/00429310.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429310 PortTop_Bound_00429310
#define FUN_00408ad0 RaceScoreFloatGetBySlot   // exe body (Match/MatchS01.cpp)
#define FUN_0040dc70 PortTop_Bound_0040dc70
#define FUN_0040dcb0 PortTop_Bound_0040dcb0
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e360 PortTop_Bound_0040e360
#define FUN_00411350 PortTop_Bound_00411350   // exe body (Match/PortTop_00411350.cpp)
#define FUN_00411580 PortTop_RFStop_00411580   // named stop
#define FUN_004115c0 PortTop_Bound_004115c0   // exe body (Match/PortTop_004115c0.cpp)
#define FUN_00411870 PortTop_RFStop_00411870   // named stop
#define FUN_0042b8c0 ScreenHeightGet   // exe body (Match/MatchS07_0042b8c0.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00430b00 TimeDisplaySetEntry   // exe body (Match/MatchS07_00430b00.cpp)
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
extern "C" {
#include "../../../../re/match/s06/00429310.c"
}
#endif // MASHED_STANDALONE
