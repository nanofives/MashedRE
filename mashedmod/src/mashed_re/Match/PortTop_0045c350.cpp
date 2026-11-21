// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0045c350 = re/match/s10/0045c350.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045c350 PortTop_Bound_0045c350
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0044d5e0 PortTop_Bound_0044d5e0
#define FUN_00452140 PortTop_RFStop_00452140   // named stop
#define FUN_0045c110 PortTop_Bound_0045c110
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
extern "C" {
#include "../../../../re/match/s10/0045c350.c"
}
#endif // MASHED_STANDALONE
