// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0055ae70 = re/match/rwl05/0055ae70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0055ae70 PortTop_Bound_0055ae70
#define FUN_00564c80 PortTop_RFStop_00564c80   // named stop
#define FUN_00565200 PortTop_RFStop_00565200   // named stop
#define FUN_00565260 PortTop_RFStop_00565260   // named stop
#define FUN_00565570 PortTop_Bound_00565570
extern "C" {
#include "../../../../re/match/rwl05/0055ae70.c"
}
#endif // MASHED_STANDALONE
