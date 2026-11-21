// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00411ae0 = re/match/racepath/00411ae0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411ae0 PortTop_Bound_00411ae0
#define FUN_0041a9b0 PortTop_RFStop_0041a9b0   // named stop
#define FUN_0041ad00 PortTop_RFStop_0041ad00   // named stop
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00482c10 PortTop_RFStop_00482c10   // named stop
extern "C" {
#include "../../../../re/match/racepath/00411ae0.c"
}
#endif // MASHED_STANDALONE
