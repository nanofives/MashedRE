// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0042a5d0 = re/match/s06/0042a5d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0042a5d0 PortTop_Bound_0042a5d0
#define FUN_0042a530 PortTop_Bound_0042a530
#define FUN_004b3bf0 MatchS06_0042a5d0_Call004b3bf0   // Match/MatchS06_0042a5d0.cpp: the source passes 1 argument, Orig_004b3bf0 takes 3; declared deviation there, 2nd / 3rd passed as 0
extern "C" {
#include "../../../../re/match/s06/0042a5d0.c"
}
#endif // MASHED_STANDALONE
