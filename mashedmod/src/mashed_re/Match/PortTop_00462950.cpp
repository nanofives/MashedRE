// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00462950 = re/match/s10/00462950.c
// (L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00462950 PortTop_Bound_00462950
#define FUN_0041e980 PortTop_Bound_0041e980
#define FUN_0045de80 PortTop_RLStop_0045de80   // named stop
#define FUN_0045e160 PortTop_Bound_0045e160
#define FUN_0045e2a0 PortTop_Bound_0045e2a0
#define FUN_004a2b60 MashedCrt_sprintf_004a2b60   // exe body (Match/PortTop_CrtStdio.cpp)
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
#define FUN_004cc160 PortTop_RwStreamClose_004cc160   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_005a6710 PortTop_Bound_005a6710   // exe body (Match/PortTop_005a6710.cpp)
#define FUN_005a6dc0 PortTop_Bound_005a6dc0   // exe body (Match/PortTop_005a6dc0.cpp)
#define FUN_005a7aa0 PortTop_Bound_005a7aa0   // exe body (Match/PortTop_005a7aa0.cpp)
#define FUN_005a7af0 PortTop_Bound_005a7af0   // exe body (Match/PortTop_005a7af0.cpp)
#define FUN_005a7b60 PortTop_Bound_005a7b60   // exe body (Match/PortTop_005a7b60.cpp)
extern "C" {
#include "../../../../re/match/s10/00462950.c"
}
#endif // MASHED_STANDALONE
