// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047ff70 = re/match/s13/0047ff70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047ff70 PortTop_Bound_0047ff70
#define FUN_004c0b30 RwCore_004c0b30   // exe body (RwCore/RwCore_004c0b30.cpp)
#define FUN_004e4380 PortTop_Bound_004e4380
#define FUN_004e69a0 RwCore_004e69a0   // exe body (RwCore/RwCore_004e69a0.cpp)
#define FUN_004e7e30 Orig_004e7e30   // exe body (Match/PortTop_004e7e30.cpp)
#define FUN_00559c40 PortTop_RLStop_00559c40   // named stop
#define FUN_0055b940 PortTop_RLStop_0055b940   // named stop
#define FUN_0055c4a0 PortTop_Bound_0055c4a0
#define FUN_0055c4f0 PortTop_Bound_0055c4f0
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_00562fd0 PortTop_Bound_00562fd0
#define FUN_0057c220 PortTop_Bound_0057c220
#define FUN_0057c300 PortTop_Bound_0057c300
#define FUN_0057c500 PortTop_Bound_0057c500
#define FUN_0057c550 PortTop_Bound_0057c550
extern "C" {
#include "../../../../re/match/s13/0047ff70.c"
}
#endif // MASHED_STANDALONE
