// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004ea220 = re/match/rwl02/004ea220.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004ea220 PortTop_Bound_004ea220
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
#define FUN_004e4f70 RwCore_004e4f70   // exe body (RwCore/RwCore_004e4f70.cpp)
#define FUN_004e5020 RwCore_004e5020   // exe body (RwCore/RwCore_004e5020.cpp)
#define FUN_004e9e40 PortTop_Bound_004e9e40
extern "C" {
#include "../../../../re/match/rwl02/004ea220.c"
}
#endif // MASHED_STANDALONE
