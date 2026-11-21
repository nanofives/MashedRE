// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00490500 = re/match/s15/00490500.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00490500 PortTop_Bound_00490500
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00472690 PortTop_Bound_00472690   // exe body (Match/PortTop_00472690.cpp)
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a30 ParticleEmitter_SetScalar   // exe body (Match/MatchS12_00476a30.cpp)
#define FUN_00476a40 ParticleEmitter_SetRGBA   // exe body (Render/RfxEmitter.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_0048ebc0 PortTop_Bound_0048ebc0
#define FUN_0048ebf0 PortTop_RFStop_0048ebf0   // named stop
#define FUN_0048f290 PortTop_Bound_0048f290
#define FUN_0048f420 PortTop_Bound_0048f420
#define FUN_0048fe70 PortTop_Bound_0048fe70
#define FUN_004a2c48 MashedCrt_ftol2_004a2c48   // exe body (Match/PortTop_CrtHelpers.cpp)
extern "C" {
#include "../../../../re/match/s15/00490500.c"
}
#endif // MASHED_STANDALONE
