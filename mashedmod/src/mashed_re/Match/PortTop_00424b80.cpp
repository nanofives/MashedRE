// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00424b80 = re/match/s05/00424b80.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00424b80 PortTop_Bound_00424b80
#define FUN_0040b6d0 UtilArrayGet8a94e0   // exe body (Match/MatchS02_0040b6d0.cpp)
#define FUN_0040e340 GetLiveCarCount   // exe body (Match/MatchS02_0040e340.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
extern "C" {
#include "../../../../re/match/s05/00424b80.c"
}
#endif // MASHED_STANDALONE
