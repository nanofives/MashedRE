// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00483a70 = re/match/s13/00483a70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00483a70 PortTop_Bound_00483a70
#define FUN_00482ae0 SplineCubicBlend3   // exe body (Vehicle/SplineCubicBlend.cpp)
#define FUN_004b59c0 PortTop_Bound_004b59c0
#define FUN_004b5af0 PortTop_RRStop_004b5af0   // named stop
extern "C" {
#include "../../../../re/match/s13/00483a70.c"
}
#endif // MASHED_STANDALONE
