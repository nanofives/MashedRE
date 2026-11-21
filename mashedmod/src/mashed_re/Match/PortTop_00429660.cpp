// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00429660 = re/match/s06/00429660.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00429660 PortTop_Bound_00429660
#define FUN_0040dc80 PortTop_Bound_0040dc80
#define FUN_0040dc90 PortTop_Bound_0040dc90
#define FUN_00427e00 RwCore_00427e00   // exe body (RwCore/RwCore_00427e00.cpp)
#define FUN_00429620 PortTop_Bound_00429620
#define FUN_0042c7c0 PortTop_Bound_0042c7c0
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
extern "C" {
#include "../../../../re/match/s06/00429660.c"
}
#endif // MASHED_STANDALONE
