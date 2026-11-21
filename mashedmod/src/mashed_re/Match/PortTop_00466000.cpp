// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00466000 = re/match/s10/00466000.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00466000 PortTop_Bound_00466000
#define FUN_0045e0f0 PortTop_Bound_0045e0f0   // exe body (Match/PortTop_0045e0f0.cpp)
#define FUN_00492d10 PortTop_Bound_00492d10   // exe body (Match/PortTop_00492d10.cpp)
#define FUN_005a6d60 PortTop_Bound_005a6d60   // exe body (Match/PortTop_005a6d60.cpp)
#define FUN_005a6dc0 PortTop_Bound_005a6dc0   // exe body (Match/PortTop_005a6dc0.cpp)
extern "C" {
#include "../../../../re/match/s10/00466000.c"
}
#endif // MASHED_STANDALONE
