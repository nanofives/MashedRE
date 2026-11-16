// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047bf70 = re/match/s13/0047bf70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047bf70 PortTop_Bound_0047bf70
#define FUN_0047be80 PortTop_RLStop_0047be80   // named stop
#define FUN_004b53b0 PortTop_RLStop_004b53b0   // named stop
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c39b0 RwCore_004c39b0   // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004c3d90 PortTop_RLStop_004c3d90   // named stop
#define FUN_0047bc90 PortTop_Local_0047bf70_0047bc90   // file-local in this source
#define FUN_0047bcc0 PortTop_Local_0047bf70_0047bcc0   // file-local in this source
extern "C" {
#include "../../../../re/match/s13/0047bf70.c"
}
#endif // MASHED_STANDALONE
