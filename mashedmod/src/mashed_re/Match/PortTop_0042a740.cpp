// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x0042a740 = re/match/s06/0042a740.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0042a740 PortTop_Bound_0042a740
#define _strchr MashedCrt_strchr_004a2fd0   // the original CRT strchr 0x004a2fd0 (Match/PortTop_CrtString.cpp, BIND-CRT-STRING)
#define FUN_0042a530 PortTop_Bound_0042a530
#define FUN_004cc160 PortTop_RwStreamClose_004cc160   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#define FUN_00558df0 PortTop_Bound_00558df0
extern "C" {
#include "../../../../re/match/s06/0042a740.c"
}
#endif // MASHED_STANDALONE
