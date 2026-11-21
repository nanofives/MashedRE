// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00426640 = re/match/s06/00426640.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00426640 PortTop_Bound_00426640
#define FUN_0041e950 PortTop_Bound_0041e950
#define FUN_0041e9b0 PortTop_Bound_0041e9b0   // exe body (Match/PortTop_0041e9b0.cpp)
#define FUN_0041ea70 PortTop_Bound_0041ea70
extern "C" {
#include "../../../../re/match/s06/00426640.c"
}
#endif // MASHED_STANDALONE
