// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047f840 = re/match/s13/0047f840.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047f840 PortTop_Bound_0047f840
#define FUN_0047f4c0 PortTop_Bound_0047f4c0
#define FUN_0047f7d0 PortTop_RLStop_0047f7d0   // named stop
#define FUN_0055ad20 PortTop_Bound_0055ad20
#define FUN_0055bac0 PortTop_Bound_0055bac0
#define FUN_0055c490 PortTop_Bound_0055c490
#define FUN_0055c4a0 PortTop_Bound_0055c4a0
#define FUN_0055c4f0 PortTop_Bound_0055c4f0
#define FUN_0055ddd0 PortTop_Bound_0055ddd0
#define FUN_0055deb0 RwCore_0055deb0   // exe body (RwCore/RwCore_0055deb0.cpp)
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
#define FUN_0055fe10 PortTop_Bound_0055fe10
#define FUN_0055fe30 PortTop_Bound_0055fe30
#define FUN_0055fe40 PortTop_Bound_0055fe40
#define FUN_00562600 PortTop_Bound_00562600
extern "C" {
#include "../../../../re/match/s13/0047f840.c"
}
#endif // MASHED_STANDALONE
