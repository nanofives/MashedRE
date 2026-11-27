// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004b53b0 = re/match/s18/004b53b0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b53b0 PortTop_Bound_004b53b0
#define FUN_004b4550 PortTop_RLStop_004b4550   // named stop
#define FUN_004c3b30 RwCore_004c3b30   // exe body (RwCore/RwCore_004c3b30.cpp)
extern "C" {
#include "../../../../re/match/s18/004b53b0.c"
}
#endif // MASHED_STANDALONE
