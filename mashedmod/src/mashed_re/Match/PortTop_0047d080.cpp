// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047d080 = re/match/s13/0047d080.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047d080 PortTop_Bound_0047d080
#define FUN_00559c40 PortTop_RLStop_00559c40   // named stop
#define FUN_00559ee0 PortTop_RLStop_00559ee0   // named stop
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_0057c210 PortTop_RLStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/0047d080.c"
}
#endif // MASHED_STANDALONE
