// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004235b0 = re/match/s05/004235b0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004235b0 PortTop_Bound_004235b0
#define FUN_00423480 PortTop_Bound_00423480
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc160 PortTop_RwStreamClose_004cc160   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc230 PortTop_RwStreamOpen_004cc230   // exe body (Match/PortTop_RwStream04F.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
extern "C" {
#include "../../../../re/match/s05/004235b0.c"
}
#endif // MASHED_STANDALONE
