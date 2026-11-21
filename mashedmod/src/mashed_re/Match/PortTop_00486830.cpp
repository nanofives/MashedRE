// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00486830 = re/match/racepath/00486830.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00486830 PortTop_Bound_00486830
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_004417e0 PortTop_Bound_004417e0   // exe body (Match/PortTop_004417e0.cpp)
#define FUN_00452140 PortTop_RFStop_00452140   // named stop
#define FUN_00465ca0 PortTop_Bound_00465ca0
#define FUN_00466000 PortTop_Bound_00466000
#define FUN_00466100 PortTop_Bound_00466100
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a10 Copy476a10   // exe body (Match/MatchS12_00476a10.cpp)
#define FUN_00476a40 ParticleEmitter_SetRGBA   // exe body (Render/RfxEmitter.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_00484cf0 PortTop_Bound_00484cf0
#define FUN_00486630 PortTop_RFStop_00486630   // named stop
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
extern "C" {
#include "../../../../re/match/racepath/00486830.c"
}
#endif // MASHED_STANDALONE
