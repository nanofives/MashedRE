// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004b6640 = re/match/s18/004b6640.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b6640 PortTop_Bound_004b6640
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_004b6630 PortTop_Bound_004b6630
#define FUN_004b6ba0 PortTop_Bound_004b6ba0
#define FUN_004b6c00 PortTop_Bound_004b6c00
#define FUN_004b6cd0 PortTop_Bound_004b6cd0
#define FUN_004b6ed0 PortTop_Bound_004b6ed0
#define FUN_004b6f60 PortTop_Bound_004b6f60
#define FUN_004b6f90 PortTop_Bound_004b6f90
#define FUN_00550430 PortTop_Bound_00550430   // exe body (Match/PortTop_00550430.cpp)
#define FUN_005504d0 PortTop_Bound_005504d0   // exe body (Match/PortTop_005504d0.cpp)
#define FUN_00551330 PortTop_Bound_00551330   // exe body (Match/PortTop_00551330.cpp)
extern "C" {
#include "../../../../re/match/s18/004b6640.c"
}
#endif // MASHED_STANDALONE
