// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00408610 = re/match/s01/00408610.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00408610 PortTop_Bound_00408610
#define FUN_00407a90 PortTop_RFStop_00407a90   // named stop
#define FUN_00407b00 PortTop_Bound_00407b00
#define FUN_00407b70 PortTop_Bound_00407b70
#define FUN_00407e20 PortTop_RFStop_00407e20   // named stop
#define FUN_00408590 PortTop_Bound_00408590
#define FUN_00426bb0 Global66d6d8Get   // exe body (Match/MatchS06_00426bb0.cpp)
#define FUN_00426cf0 GetDat0066d6e4   // exe body (Match/MatchS06_00426cf0.cpp)
#define FUN_0046cbb0 PortTop_RFStop_0046cbb0   // named stop
#define FUN_00538c80 PortTop_RFStop_00538c80   // named stop
extern "C" {
#include "../../../../re/match/s01/00408610.c"
}
#endif // MASHED_STANDALONE
