// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x004e5680 = re/match/rwl02/004e5680.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e5680 PortTop_Bound_004e5680
#define FUN_004c1b40 PortTop_RRStop_004c1b40   // named stop
#define FUN_004e6100 RwCore_004e6100   // exe body (RwCore/RwCore_004e6100.cpp)
#define FUN_004f0900 PortTop_Bound_004f0900
extern "C" {
#include "../../../../re/match/rwl02/004e5680.c"
}
#endif // MASHED_STANDALONE
