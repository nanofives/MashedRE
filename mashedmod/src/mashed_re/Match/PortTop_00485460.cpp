// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00485460 = re/match/s14/00485460.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00485460 PortTop_Bound_00485460
#define FUN_00451730 PortTop_RLStop_00451730   // named stop
#define FUN_004c1800 Orig_004c1800   // exe body (Match/PortTop_004c1800.cpp)
#define FUN_004c1840 Orig_004c1840   // exe body (Match/PortTop_004c1840.cpp)
#define FUN_004c3d90 RwCoreLift_004c3d90   // exe body (RwCore/RwCore_Lifted_00.cpp)
extern "C" {
#include "../../../../re/match/s14/00485460.c"
}
#endif // MASHED_STANDALONE
