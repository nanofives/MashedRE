// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004298e0 = re/match/s06/004298e0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004298e0 PortTop_Bound_004298e0
#define FUN_00411580 PortTop_RFStop_00411580   // named stop
#define FUN_00430b00 TimeDisplaySetEntry   // exe body (Match/MatchS07_00430b00.cpp)
extern "C" {
#include "../../../../re/match/s06/004298e0.c"
}
#endif // MASHED_STANDALONE
