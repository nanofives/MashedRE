// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047a0f0 = re/match/s12/0047a0f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047a0f0 PortTop_Bound_0047a0f0
#define FUN_00440bc0 PortTop_RLStop_00440bc0   // named stop
#define FUN_0047b9b0 PortTop_Bound_0047b9b0
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
extern "C" {
#include "../../../../re/match/s12/0047a0f0.c"
}
#endif // MASHED_STANDALONE
