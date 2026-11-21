// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004864f0 = re/match/s14/004864f0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004864f0 PortTop_Bound_004864f0
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_004c5010 RwCore_004c5010   // exe body (RwCore/RwCore_004c5010.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
#define FUN_00486370 PortTop_Local_004864f0_00486370   // file-local in this source
extern "C" {
#include "../../../../re/match/s14/004864f0.c"
}
#endif // MASHED_STANDALONE
