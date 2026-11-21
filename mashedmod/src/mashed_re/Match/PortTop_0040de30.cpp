// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x0040de30 = re/match/s02/0040de30.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040de30 PortTop_Bound_0040de30
#define FUN_00426670 PortTop_Bound_00426670
#define FUN_004c1480 PortTop_Bound_004c1480   // exe body (Match/PortTop_004c1480.cpp)
#define FUN_004c1c10 RwCore_004c1c10   // exe body (RwCore/RwCore_004c1c10.cpp)
#define FUN_004c1c80 RwCore_004c1c80   // exe body (RwCore/RwCore_004c1c80.cpp)
extern "C" {
#include "../../../../re/match/s02/0040de30.c"
}
#endif // MASHED_STANDALONE
