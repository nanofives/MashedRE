// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004b59c0 = re/match/s18/004b59c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b59c0 PortTop_Bound_004b59c0
#define FUN_004cd070 RwCore_004cd070   // exe body (RwCore/RwCore_004cd070.cpp)
#define FUN_004cd140 RwCore_004cd140   // exe body (RwCore/RwCore_004cd140.cpp)
#define FUN_004cd430 PortTop_Bound_004cd430
extern "C" {
#include "../../../../re/match/s18/004b59c0.c"
}
#endif // MASHED_STANDALONE
