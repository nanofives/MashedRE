// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048b440 = re/match/racepath/0048aff0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048b440 PortTop_Bound_0048b440
#define FUN_00465ca0 PortTop_Bound_00465ca0
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00472740 PortTop_Bound_00472740
#define FUN_00474de0 PortTop_Bound_00474de0
#define FUN_00476320 PortTop_Bound_00476320
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a30 ParticleEmitter_SetScalar   // exe body (Match/MatchS12_00476a30.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c1520 PortTop_Bound_004c1520   // exe body (Match/PortTop_004c1520.cpp)
#define FUN_00544bf0 PortTop_RFStop_00544bf0   // named stop
#define FUN_0048aff0 PortTop_Local_0048b440_0048aff0   // file-local in this source
#define FUN_0048b230 PortTop_Local_0048b440_0048b230   // file-local in this source
extern "C" {
#include "../../../../re/match/racepath/0048aff0.c"
}
#endif // MASHED_STANDALONE
