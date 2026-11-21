// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00475010 = re/match/s12/00475010.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00475010 PortTop_Bound_00475010
#define FUN_00474f50 PortTop_RFStop_00474f50   // named stop
#define FUN_00474fb0 PortTop_Bound_00474fb0   // exe body (Match/PortTop_00474fb0.cpp)
extern "C" {
#include "../../../../re/match/s12/00475010.c"
}
#endif // MASHED_STANDALONE
