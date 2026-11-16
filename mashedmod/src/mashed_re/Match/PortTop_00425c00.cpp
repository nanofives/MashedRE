// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00425c00 = re/match/s05/00425c00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00425c00 PortTop_Bound_00425c00
#define FUN_00425b90 PortTop_Bound_00425b90
#define FUN_004781b0 Orig_004781b0   // exe body (Match/PortTop_004781b0.cpp)
#define FUN_004a2b60 MashedCrt_sprintf_004a2b60   // exe body (Match/PortTop_CrtStdio.cpp)
#define FUN_004c5c80 RwCore_004c5c80   // exe body (RwCore/RwCore_004c5c80.cpp)
extern "C" {
#include "../../../../re/match/s05/00425c00.c"
}
#endif // MASHED_STANDALONE
