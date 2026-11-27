// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00551460 = re/match/rwl04/00551460.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00551460 PortTop_Bound_00551460
extern "C" {
#include "../../../../re/match/rwl04/00551460.c"
}
#endif // MASHED_STANDALONE
