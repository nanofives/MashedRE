// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00422120 = re/match/s05/00422120.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00422120 PortTop_Bound_00422120
#define FUN_00421c10 PortTop_Bound_00421c10
#define FUN_00421c50 PortTop_Local_00422120_00421c50   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00422120.c"
}
#endif // MASHED_STANDALONE
