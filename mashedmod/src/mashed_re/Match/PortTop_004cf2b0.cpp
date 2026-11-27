// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004cf2b0 = re/match/rwl01/004cf2b0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004cf2b0 PortTop_Bound_004cf2b0
#define FUN_004c4670 RwCore_004c4670   // exe body (RwCore/RwCore_004c4670.cpp)
#define FUN_004c57a0 RwCore_004c57a0   // exe body (RwCore/RwCore_004c57a0.cpp)
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
extern "C" {
#include "../../../../re/match/rwl01/004cf2b0.c"
}
#endif // MASHED_STANDALONE
