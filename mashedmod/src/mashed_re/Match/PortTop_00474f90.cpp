// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00474f90 = re/match/tlr/00474f90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00474f90 PortTop_Bound_00474f90
#define FUN_004e89a0 PortTop_RLStop_004e89a0   // named stop
extern "C" {
#include "../../../../re/match/tlr/00474f90.c"
}
#endif // MASHED_STANDALONE
