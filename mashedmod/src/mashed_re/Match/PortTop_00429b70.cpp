// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00429b70 = re/match/s06/00429b70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429b70 PortTop_Bound_00429b70
#define FUN_0042b8c0 ScreenHeightGet   // exe body (Match/MatchS07_0042b8c0.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
extern "C" {
#include "../../../../re/match/s06/00429b70.c"
}
#endif // MASHED_STANDALONE
