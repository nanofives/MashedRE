// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040e560 = re/match/s02/0040e560.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040e560 PortTop_Bound_0040e560
#define FUN_0040e360 PortTop_Bound_0040e360
#define FUN_00411870 PortTop_RFStop_00411870   // named stop
#define FUN_0041e130 TimerTrackSetter   // exe body (Match/MatchS04_0041e130.cpp)
#define FUN_00429840 RaceStateLatchSet   // exe body (Match/MatchS06_00429840.cpp)
#define FUN_004298e0 PortTop_Bound_004298e0
extern "C" {
#include "../../../../re/match/s02/0040e560.c"
}
#endif // MASHED_STANDALONE
