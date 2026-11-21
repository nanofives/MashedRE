// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004212b0 = re/match/s05/004212b0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004212b0 PortTop_Bound_004212b0
#define FUN_0040e370 IsCarSlotActive   // exe body (Match/MatchS02_0040e370.cpp)
#define FUN_0041f290 PortTop_Bound_0041f290
#define FUN_00421160 PortTop_Bound_00421160
#define FUN_0046cb30 PortTop_Bound_0046cb30
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00474de0 PortTop_Bound_00474de0
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a30 ParticleEmitter_SetScalar   // exe body (Match/MatchS12_00476a30.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
extern "C" {
#include "../../../../re/match/s05/004212b0.c"
}
#endif // MASHED_STANDALONE
