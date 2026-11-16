// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00480340 = re/match/s13/00480340.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00480340 PortTop_Bound_00480340
#define FUN_00426060 PortTop_Bound_00426060
#define FUN_00426070 TrackPhysWorld2Get   // exe body (Match/MatchS06_00426070.cpp)
#define FUN_0044d6e0 PortTop_Bound_0044d6e0
#define FUN_00478110 PortTop_Bound_00478110
#define FUN_004781b0 Orig_004781b0   // exe body (Match/PortTop_004781b0.cpp)
#define FUN_0047f840 PortTop_Bound_0047f840
#define FUN_0047f940 PortTop_Bound_0047f940
#define FUN_0047fc40 PortTop_Bound_0047fc40
#define FUN_0047fe00 PortTop_Bound_0047fe00
#define FUN_0047ff70 PortTop_Bound_0047ff70
#define FUN_00480100 PortTop_Bound_00480100
#define FUN_004b3f50 PortTop_RLStop_004b3f50   // named stop
#define FUN_004b3f90 RwCore_004b3f90   // exe body (RwCore/RwCore_004b3f90.cpp)
#define FUN_004e6e00 RwCore_004e6e00   // exe body (RwCore/RwCore_004e6e00.cpp)
extern "C" {
#include "../../../../re/match/s13/00480340.c"
}
#endif // MASHED_STANDALONE
