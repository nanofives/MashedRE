// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00401f10 = re/match/s01/00401f10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00401f10 PortTop_Bound_00401f10
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_00412130 PortTop_RRStop_00412130   // named stop
#define FUN_004e6680 PortTop_Bound_004e6680   // exe body (Match/PortTop_004e6680.cpp)
extern "C" {
#include "../../../../re/match/s01/00401f10.c"
}
#endif // MASHED_STANDALONE
