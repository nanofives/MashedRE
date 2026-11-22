// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0041f3d0 = re/match/s04/0041f3d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041f3d0 PortTop_Bound_0041f3d0
#define FUN_0046bd60 PortTop_Bound_0046bd60
#define FUN_0046bda0 PortTop_Bound_0046bda0
#define FUN_004c1340 PortTop_Bound_004c1340   // exe body (Match/PortTop_004c1340.cpp)
#define FUN_004c1520 PortTop_Bound_004c1520   // exe body (Match/PortTop_004c1520.cpp)
extern "C" {
#include "../../../../re/match/s04/0041f3d0.c"
}
#endif // MASHED_STANDALONE
