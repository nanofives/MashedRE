// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048a5d0 = re/match/s14/0048a490.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048a5d0 PortTop_Bound_0048a5d0
#define FUN_00474d80 PortTop_Bound_00474d80
#define FUN_004b40f0 PortTop_Bound_004b40f0
#define FUN_004b5240 Orig_004b5240   // exe body (Match/PortTop_004b5240.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_0048a490 PortTop_Local_0048a5d0_0048a490   // file-local in this source
extern "C" {
#include "../../../../re/match/s14/0048a490.c"
}
#endif // MASHED_STANDALONE
