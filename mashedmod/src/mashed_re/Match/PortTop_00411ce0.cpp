// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00411ce0 = re/match/s02/00411ce0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411ce0 PortTop_Bound_00411ce0
#define FUN_0041a960 PortTop_RRStop_0041a960   // named stop
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00483a70 PortTop_Bound_00483a70
extern "C" {
#include "../../../../re/match/s02/00411ce0.c"
}
#endif // MASHED_STANDALONE
