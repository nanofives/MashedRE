// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004818d0 = re/match/s13/004818d0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004818d0 PortTop_Bound_004818d0
#define FUN_00453eb0 PortTop_Bound_00453eb0
#define FUN_00476860 PortTop_Bound_00476860
#define FUN_0047d080 PortTop_Bound_0047d080   // exe body (Match/PortTop_0047d080.cpp)
#define FUN_0047d100 PortTop_Bound_0047d100   // exe body (Match/PortTop_0047d100.cpp)
#define FUN_0048a4e0 PortTop_Bound_0048a4e0
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/004818d0.c"
}
#endif // MASHED_STANDALONE
