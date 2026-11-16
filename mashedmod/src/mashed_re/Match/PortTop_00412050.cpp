// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00412050 = re/match/s02/00412050.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00412050 PortTop_Bound_00412050
#define FUN_0047cdc0 PortTop_Bound_0047cdc0
#define FUN_0047fad0 PortTop_Bound_0047fad0
#define FUN_004b3f90 RwCore_004b3f90   // exe body (RwCore/RwCore_004b3f90.cpp)
extern "C" {
#include "../../../../re/match/s02/00412050.c"
}
#endif // MASHED_STANDALONE
