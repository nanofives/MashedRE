// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0045e160 = re/match/s10/0045e160.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045e160 PortTop_Bound_0045e160
#define FUN_00546380 PortTop_Bound_00546380
extern "C" {
#include "../../../../re/match/s10/0045e160.c"
}
#endif // MASHED_STANDALONE
