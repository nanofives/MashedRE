// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048f9f0 = re/match/s15/0048f9f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048f9f0 PortTop_Bound_0048f9f0
#define FUN_004769a0 ParticleEmitter_SetPosition   // exe body (Match/MatchS12_004769a0.cpp)
#define FUN_004769d0 ParticleEmitter_SetVelocity   // exe body (Match/MatchS12_004769d0.cpp)
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a40 ParticleEmitter_SetRGBA   // exe body (Render/RfxEmitter.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_00476df0 PortTop_Bound_00476df0
extern "C" {
#include "../../../../re/match/s15/0048f9f0.c"
}
#endif // MASHED_STANDALONE
