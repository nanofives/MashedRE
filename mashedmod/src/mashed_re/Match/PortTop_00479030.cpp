// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00479030 = re/match/s12/00479030.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00479030 PortTop_Bound_00479030
#define FUN_004790e0 PortTop_RLStop_004790e0   // named stop
#define FUN_004b4550 PortTop_RLStop_004b4550   // named stop
#define FUN_004b53b0 PortTop_Bound_004b53b0
#define FUN_004e5c70 RwCore_004e5c70   // exe body (RwCore/RwCore_004e5c70.cpp)
#define FUN_00543da0 PortTop_Bound_00543da0
#define FUN_00543dc0 PortTop_Bound_00543dc0
#define FUN_00543e30 PortTop_Bound_00543e30
extern "C" {
#include "../../../../re/match/s12/00479030.c"
}
#endif // MASHED_STANDALONE
