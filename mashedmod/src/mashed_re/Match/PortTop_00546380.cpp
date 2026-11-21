// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00546380 = re/match/rwl04/00546380.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00546380 PortTop_Bound_00546380
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
#define FUN_00545340 PortTop_RLStop_00545340   // named stop
extern "C" {
#include "../../../../re/match/rwl04/00546380.c"
}
#endif // MASHED_STANDALONE
