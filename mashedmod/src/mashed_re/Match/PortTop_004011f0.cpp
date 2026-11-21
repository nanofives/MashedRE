// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004011f0 = re/match/s01/004011f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004011f0 PortTop_Bound_004011f0
#define FUN_00429300 HudOverlayFloatGet   // exe body (Match/MatchS06_00429300.cpp)
#define FUN_004b42c0 PortTop_RRStop_004b42c0   // named stop
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c13e0 RwCore_004c13e0   // exe body (RwCore/RwCore_004c13e0.cpp)
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004c1520 PortTop_Bound_004c1520   // exe body (Match/PortTop_004c1520.cpp)
#define FUN_004c15c0 RwCore_004c15c0   // exe body (RwCore/RwCore_004c15c0.cpp)
extern "C" {
#include "../../../../re/match/s01/004011f0.c"
}
#endif // MASHED_STANDALONE
