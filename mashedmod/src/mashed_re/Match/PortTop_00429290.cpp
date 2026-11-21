// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00429290 = re/match/s06/00429290.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429290 PortTop_Bound_00429290
#define FUN_00428a30 PortTop_Bound_00428a30
#define FUN_00428bf0 PortTop_Bound_00428bf0
#define FUN_00428d30 PortTop_Bound_00428d30
#define FUN_0045d3f0 PortTop_Bound_0045d3f0   // exe body (Match/PortTop_0045d3f0.cpp)
#define FUN_0045d430 PortTop_Bound_0045d430   // exe body (Match/PortTop_0045d430.cpp)
#define FUN_00466b50 PortTop_Bound_00466b50   // exe body (Match/PortTop_00466b50.cpp)
#define FUN_004967e0 PortTop_Local_00496530_004967e0   // exe body (Match/PortTop_00496530.cpp)
#define FUN_00429240 PortTop_Local_00429290_00429240   // file-local in this source
extern "C" {
#include "../../../../re/match/s06/00429290.c"
}
#endif // MASHED_STANDALONE
