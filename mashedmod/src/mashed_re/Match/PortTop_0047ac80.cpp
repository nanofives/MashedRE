// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0047ac80 = re/match/s13/0047ac80.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047ac80 PortTop_Bound_0047ac80
#define FUN_004b6fc0 lua_gettop   // exe body (lib:lua-4.0:src/lapi.c:68)
#define FUN_004b7090 lua_tonumber   // exe body (lib:lua-4.0:src/lapi.c:157)
extern "C" {
#include "../../../../re/match/s13/0047ac80.c"
}
#endif // MASHED_STANDALONE
