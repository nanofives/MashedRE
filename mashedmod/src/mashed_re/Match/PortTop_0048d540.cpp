// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048d540 = re/match/s15/0048d540.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048d540 PortTop_Bound_0048d540
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a30 ParticleEmitter_SetScalar   // exe body (Match/MatchS12_00476a30.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_0048d0f0 PortTop_Local_0048d540_0048d0f0   // file-local in this source
extern "C" {
#include "../../../../re/match/s15/0048d540.c"
}
#endif // MASHED_STANDALONE
