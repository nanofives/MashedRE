// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004c1b40 = re/match/s19/004c1b40.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c1b40 PortTop_Bound_004c1b40
extern "C" {
#include "../../../../re/match/s19/004c1b40.c"
}
#endif // MASHED_STANDALONE
