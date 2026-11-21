// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00433240 = re/match/s08/00433240.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00433240 PortTop_Bound_00433240
#define FUN_00402f80 CupFloatInit   // exe body (Match/MatchS01.cpp)
#define FUN_0042d3a0 RaceTransitionBufZero   // exe body (Match/MatchS07_0042d3a0.cpp)
extern "C" {
#include "../../../../re/match/s08/00433240.c"
}
#endif // MASHED_STANDALONE
