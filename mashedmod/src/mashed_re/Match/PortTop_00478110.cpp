// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00478110 = re/match/s12/00478110.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00478110 PortTop_Bound_00478110
#define FUN_004cc160 PortTop_RwStreamClose_004cc160   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004cc790 RwCore_004cc790   // exe body (RwCore/RwCore_004cc790.cpp)
#define FUN_004cf2b0 PortTop_RLStop_004cf2b0   // named stop
extern "C" {
#include "../../../../re/match/s12/00478110.c"
}
#endif // MASHED_STANDALONE
