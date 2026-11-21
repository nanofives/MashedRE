// PORT-TOP-RENDERFRAME-492E90 (2026-11-21): exe-only port body of rva 0x00444ff0 = re/match/s08/00444ff0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RENDERFRAME
// through the frame-render closure of 0x00492e90 (re/tools/bind/porttop_bind.py --profile renderframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RenderFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00444ff0 PortTop_Bound_00444ff0
#define FUN_00408a50 PerCarRaceProgressGet   // exe body (Match/MatchS01.cpp)
#define FUN_004173e0 PortTop_RRStop_004173e0   // named stop
#define FUN_004430e0 PortTop_Bound_004430e0
#define FUN_00443210 PortTop_Bound_00443210
#define FUN_00444b60 PortTop_RRStop_00444b60   // named stop
#define FUN_005586f0 PortTop_RRStop_005586f0   // named stop
extern "C" {
#include "../../../../re/match/s08/00444ff0.c"
}
#endif // MASHED_STANDALONE
