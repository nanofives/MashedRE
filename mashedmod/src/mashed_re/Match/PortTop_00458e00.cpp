// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00458e00 = re/match/s09/00458e00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00458e00 PortTop_Bound_00458e00
#define FUN_0042fe30 RaceEndFlagIfEndMode   // exe body (Match/MatchS07_0042fe30.cpp)
#define FUN_00458d00 PortTop_Bound_00458d00
#define FUN_00458dd0 PortTop_RLStop_00458dd0   // named stop
#define FUN_004c15c0 RwCore_004c15c0   // exe body (RwCore/RwCore_004c15c0.cpp)
extern "C" {
#include "../../../../re/match/s09/00458e00.c"
}
#endif // MASHED_STANDALONE
