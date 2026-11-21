// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0040cea0 = re/match/s02/0040cea0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040cea0 PortTop_Bound_0040cea0
#define FUN_00404e00 IdxPtr404e00   // exe body (Vehicle/CarMaterialTables.cpp)
#define FUN_00404e20 IdxPtr404e20   // exe body (Vehicle/CarMaterialTables.cpp)
#define FUN_0041ecc0 PortTop_Bound_0041ecc0   // exe body (Match/PortTop_0041ecc0.cpp)
#define FUN_0042a8d0 PortTop_Bound_0042a8d0
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004c5c80 RwCore_004c5c80   // exe body (RwCore/RwCore_004c5c80.cpp)
#define FUN_004c5cb0 RwCore_004c5cb0   // exe body (RwCore/RwCore_004c5cb0.cpp)
extern "C" {
#include "../../../../re/match/s02/0040cea0.c"
}
extern "C" { void *PortTop_Keep_0040cea0 = (void *)PortTop_Bound_0040cea0; }   // anchor: Match/PortTop_BulkRefs.cpp /include (no bound caller yet)
#endif // MASHED_STANDALONE
