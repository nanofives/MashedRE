// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004e99b0 = re/match/rwl02/004e99b0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e99b0 PortTop_Bound_004e99b0
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8000 RwCore_004d8000   // exe body (RwCore/RwCore_004d8000.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
#define FUN_004e1b60 RwCore_004e1b60   // exe body (RwCore/RwCore_004e1b60.cpp)
#define FUN_004e1c90 RwCore_004e1c90   // exe body (RwCore/RwCore_004e1c90.cpp)
#define FUN_004e51e0 PortTop_Bound_004e51e0
#define FUN_004e5280 PortTop_Bound_004e5280
#define FUN_004e5300 RwCore_004e5300   // exe body (RwCore/RwCore_004e5300.cpp)
#define FUN_004e5700 RwCore_004e5700   // exe body (RwCore/RwCore_004e5700.cpp)
#define FUN_004e5820 RwCore_004e5820   // exe body (RwCore/RwCore_004e5820.cpp)
#define FUN_004e8090 Orig_004e8090   // exe body (Match/PortTop_004e8090.cpp)
#define FUN_004e9e40 PortTop_Bound_004e9e40
#define FUN_004ea220 PortTop_Bound_004ea220
#define FUN_004f3e90 RwCoreLift_004f3e90   // exe body (RwCore/RwCore_Lifted_04.cpp)
extern "C" {
#include "../../../../re/match/rwl02/004e99b0.c"
}
#endif // MASHED_STANDALONE
