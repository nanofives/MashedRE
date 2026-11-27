// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004b6940 = re/match/s18/004b6940.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b6940 PortTop_Bound_004b6940
#define FUN_004a34b0 PortTop_PizCrt_strncpy_004a34b0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004a4541 MashedCrt_fopen_004a4541   // exe body (Match/PortTop_CrtStdio.cpp)
#define FUN_004b302f PortTop_PizCrt_stricmp_004b302f   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004b65e0 PortTop_Bound_004b65e0
#define FUN_004b67e0 PortTop_Bound_004b67e0
#define FUN_00550790 PortTop_Bound_00550790
#define FUN_005507a0 PortTop_Bound_005507a0
#define FUN_004b6710 PortTop_Local_004b6940_004b6710   // file-local in this source
extern "C" {
#include "../../../../re/match/s18/004b6940.c"
}
#endif // MASHED_STANDALONE
