// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004262f0 = re/match/s06/004262f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004262f0 PortTop_Bound_004262f0
#define FUN_00425b70 SlotFieldSetter   // exe body (Match/MatchS05_00425b70.cpp)
#define FUN_00425bf0 PortTop_RLStop_00425bf0   // named stop
#define FUN_00425c00 PortTop_Bound_00425c00
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
extern "C" {
#include "../../../../re/match/s06/004262f0.c"
}
#endif // MASHED_STANDALONE
