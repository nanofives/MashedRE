// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00420050 = re/match/s05/00420050.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00420050 PortTop_Bound_00420050
#define FUN_00422570 PortTop_Bound_00422570
#define FUN_004228f0 PortTop_RRStop_004228f0   // named stop
#define FUN_00422ac0 PortTop_Bound_00422ac0
#define FUN_00422af0 PortTop_Bound_00422af0
#define FUN_00426060 PortTop_Bound_00426060   // exe body (Match/PortTop_00426060.cpp)
#define FUN_00426070 TrackPhysWorld2Get   // exe body (Match/MatchS06_00426070.cpp)
#define FUN_004260c0 PortTop_Bound_004260c0
#define FUN_00426e00 PortTop_Bound_00426e00
#define FUN_004769f0 ParticleEmitter_SetColour   // exe body (Match/MatchS12_004769f0.cpp)
#define FUN_00476a10 Copy476a10   // exe body (Match/MatchS12_00476a10.cpp)
#define FUN_00476d00 MultiArrayScatter476d00   // exe body (Match/MatchS12_00476d00.cpp)
#define FUN_004b42c0 PortTop_RRStop_004b42c0   // named stop
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c1680 PortTop_Bound_004c1680
#define FUN_004c19f0 RwCore_004c19f0   // exe body (RwCore/RwCore_004c19f0.cpp)
#define FUN_004c1a00 RwCore_004c1a00   // exe body (RwCore/RwCore_004c1a00.cpp)
#define FUN_004c1b40 PortTop_RRStop_004c1b40   // named stop
#define FUN_004c1bb0 RwCore_004c1bb0   // exe body (RwCore/RwCore_004c1bb0.cpp)
#define FUN_004c45f0 RwCore_004c45f0   // exe body (RwCore/RwCore_004c45f0.cpp)
#define FUN_004c4d20 RwCore_004c4d20   // exe body (RwCore/RwCore_004c4d20.cpp)
#define FUN_004c5010 RwCore_004c5010   // exe body (RwCore/RwCore_004c5010.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
#define FUN_004c52f0 PortTop_RRStop_004c52f0   // named stop
#define FUN_004e4320 PortTop_Bound_004e4320
#define FUN_004e4350 PortTop_Bound_004e4350
#define FUN_004e4900 Orig_004e4900   // exe body (Match/PortTop_004e4900.cpp)
#define FUN_004e6680 PortTop_RRStop_004e6680   // named stop
#define FUN_0041f8f0 PortTop_Local_00420050_0041f8f0   // file-local in this source
#define FUN_0041faf0 PortTop_Local_00420050_0041faf0   // file-local in this source
#define FUN_0041fcc0 PortTop_Local_00420050_0041fcc0   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00420050.c"
}
#endif // MASHED_STANDALONE
