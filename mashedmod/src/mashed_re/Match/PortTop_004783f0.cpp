// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004783f0 = re/match/s12/004783f0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004783f0 PortTop_Bound_004783f0
#define FUN_00409300 PortTop_Bound_00409300
#define FUN_00409330 PortTop_Bound_00409330
#define FUN_004a3384 MashedCrt_acos_004a3384   // exe body (Match/PortTop_CrtMath.cpp)
#define FUN_004c39b0 RwCore_004c39b0   // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004c3b30 RwCore_004c3b30   // exe body (RwCore/RwCore_004c3b30.cpp)
extern "C" {
#include "../../../../re/match/s12/004783f0.c"
}
#endif // MASHED_STANDALONE
