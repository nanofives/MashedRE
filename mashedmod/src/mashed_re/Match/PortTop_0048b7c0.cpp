// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048b7c0 = re/match/s14/0048b7c0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048b7c0 PortTop_Bound_0048b7c0
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a10 Copy476a10   // exe body (Match/MatchS12_00476a10.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_004c4d20 RwCore_004c4d20   // exe body (RwCore/RwCore_004c4d20.cpp)
#define FUN_004c5010 RwCore_004c5010   // exe body (RwCore/RwCore_004c5010.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
extern "C" {
#include "../../../../re/match/s14/0048b7c0.c"
}
#endif // MASHED_STANDALONE
