// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00558cb0 = re/match/rwl05/00558cb0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00558cb0 PortTop_Bound_00558cb0
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_005c4e10 PortTop_Bound_005c4e10
extern "C" {
#include "../../../../re/match/rwl05/00558cb0.c"
}
#endif // MASHED_STANDALONE
