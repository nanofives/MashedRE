// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00558df0 = re/match/rwl05/00558df0.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00558df0 PortTop_Bound_00558df0
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_004d7ff0 Orig_004d7ff0   // exe body (Match/PortTop_004d7ff0.cpp)
#define FUN_004d8480 RwCore_004d8480   // exe body (RwCore/RwCore_004d8480.cpp)
#define FUN_0055deb0 RwCore_0055deb0   // exe body (RwCore/RwCore_0055deb0.cpp)
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_005c4ad0 RwCore_005c4ad0   // exe body (RwCore/RwCore_005c4ad0.cpp)
#define FUN_005c4bb0 RwCore_005c4bb0   // exe body (RwCore/RwCore_005c4bb0.cpp)
#define FUN_005c4d20 PortTop_Bound_005c4d20
#define FUN_005c4d50 Orig_005c4d50   // exe body (Match/PortTop_005c4d50.cpp)
#define FUN_005c4df0 RwCore_005c4df0   // exe body (RwCore/RwCore_005c4df0.cpp)
#define FUN_005c4e10 PortTop_Bound_005c4e10
extern "C" {
#include "../../../../re/match/rwl05/00558df0.c"
}
#endif // MASHED_STANDALONE
