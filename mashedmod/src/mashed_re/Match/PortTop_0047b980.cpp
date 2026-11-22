// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047b980 = re/match/s13/0047b980.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047b980 PortTop_Bound_0047b980
#define FUN_004b7200 lua_pushcclosure   // exe body (lib:lua-4.0:src/lapi.c:230)
#define FUN_004b7250 lua_setglobal   // exe body (lib:lua-4.0:src/lapi.c:315)
extern "C" {
#include "../../../../re/match/s13/0047b980.c"
}
#endif // MASHED_STANDALONE
