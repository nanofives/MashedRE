// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004073b0 = re/match/s01/004073b0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004073b0 PortTop_Bound_004073b0
#define FUN_00406370 PortTop_RFStop_00406370   // named stop
#define FUN_0047d180 PortTop_Bound_0047d180
#define FUN_004b47e0 PortTop_Bound_004b47e0
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
extern "C" {
#include "../../../../re/match/s01/004073b0.c"
}
#endif // MASHED_STANDALONE
