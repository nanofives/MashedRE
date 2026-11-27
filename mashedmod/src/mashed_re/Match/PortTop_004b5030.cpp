// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004b5030 = re/match/s18/004b5030.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b5030 PortTop_Bound_004b5030
#define FUN_004b4eb0 PortTop_Bound_004b4eb0
#define FUN_004e5c70 RwCore_004e5c70   // exe body (RwCore/RwCore_004e5c70.cpp)
extern "C" {
#include "../../../../re/match/s18/004b5030.c"
}
#endif // MASHED_STANDALONE
