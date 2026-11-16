// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047f4c0 = re/match/pilot/a5_0047f4c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047f4c0 PortTop_Bound_0047f4c0
#define FUN_0055af40 PortTop_Bound_0055af40
#define FUN_0055dc70 PortTop_Bound_0055dc70
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_00562520 PortTop_Bound_00562520
#define FUN_00562580 PortTop_Bound_00562580
#define FUN_005625e0 PortTop_Bound_005625e0
#define FUN_0057c4b0 PortTop_RLStop_0057c4b0   // named stop
extern "C" {
#include "../../../../re/match/pilot/a5_0047f4c0.c"
}
#endif // MASHED_STANDALONE
