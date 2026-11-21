// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x004264d0 = re/match/s06/004264d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004264d0 PortTop_Bound_004264d0
#define FUN_0041e980 PortTop_Bound_0041e980
#define FUN_0042a470 PortTop_Bound_0042a470
#define FUN_0042a5d0 PortTop_Bound_0042a5d0
#define FUN_0042a8d0 PortTop_Bound_0042a8d0
#define FUN_0042fe30 RaceEndFlagIfEndMode   // exe body (Match/MatchS07_0042fe30.cpp)
#define FUN_00458c70 PortTop_RL_00458c70   // exe body (Match/PortTop_StandInsRL.cpp)
#define FUN_00458fd0 PortTop_Bound_00458fd0
#define FUN_0047a130 PortTop_Bound_0047a130
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004b3fc0 Orig_004b3fc0   // exe body (Match/PortTop_004b3fc0.cpp)
#define FUN_004b5190 RwCore_004b5190   // exe body (RwCore/RwCore_004b5190.cpp)
#define FUN_004e6e00 RwCore_004e6e00   // exe body (RwCore/RwCore_004e6e00.cpp)
#define FUN_00426460 PortTop_Local_004264d0_00426460   // file-local in this source
extern "C" {
#include "../../../../re/match/s06/004264d0.c"
}
#endif // MASHED_STANDALONE
