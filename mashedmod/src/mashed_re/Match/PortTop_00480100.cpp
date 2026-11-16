// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00480100 = re/match/s13/00480100.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00480100 PortTop_Bound_00480100
#define FUN_0047d080 PortTop_Bound_0047d080
#define FUN_0047d100 PortTop_Bound_0047d100
#define FUN_00487280 PortTop_RLStop_00487280   // named stop
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#define FUN_004c45f0 RwCore_004c45f0   // exe body (RwCore/RwCore_004c45f0.cpp)
#define FUN_0055ab30 PortTop_RLStop_0055ab30   // named stop
#define FUN_0055ac00 PortTop_RLStop_0055ac00   // named stop
#define FUN_0055ade0 PortTop_Bound_0055ade0
#define FUN_0055bd80 PortTop_Bound_0055bd80
#define FUN_0055dff0 PortTop_RLStop_0055dff0   // named stop
#define FUN_0057c210 PortTop_RLStop_0057c210   // named stop
#define FUN_0057c420 PortTop_Bound_0057c420
extern "C" {
#include "../../../../re/match/s13/00480100.c"
}
#endif // MASHED_STANDALONE
