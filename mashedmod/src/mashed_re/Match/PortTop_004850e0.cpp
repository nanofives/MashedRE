// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004850e0 = re/match/s14/004850e0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004850e0 PortTop_Bound_004850e0
#define FUN_00485460 PortTop_Bound_00485460
#define FUN_004858c0 PortTop_Bound_004858c0
#define FUN_00485a00 PortTop_Bound_00485a00
#define FUN_00485a70 PortTop_Bound_00485a70
#define FUN_00485bd0 PortTop_Bound_00485bd0
#define FUN_004c0b30 RwCore_004c0b30   // exe body (RwCore/RwCore_004c0b30.cpp)
#define FUN_004c0c20 RwCore_004c0c20   // exe body (RwCore/RwCore_004c0c20.cpp)
#define FUN_004c15c0 RwCore_004c15c0   // exe body (RwCore/RwCore_004c15c0.cpp)
#define FUN_004e5fc0 RwCore_004e5fc0   // exe body (RwCore/RwCore_004e5fc0.cpp)
#define FUN_004e7e30 Orig_004e7e30   // exe body (Match/PortTop_004e7e30.cpp)
extern "C" {
#include "../../../../re/match/s14/004850e0.c"
}
#endif // MASHED_STANDALONE
