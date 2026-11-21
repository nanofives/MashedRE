// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048a4e0 = re/match/s14/0048a4e0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048a4e0 PortTop_Bound_0048a4e0
#define FUN_00472650 PortTop_Bound_00472650   // exe body (Match/PortTop_00472650.cpp)
#define FUN_00490020 PortTop_RFStop_00490020   // named stop
#define FUN_004b40f0 PortTop_Bound_004b40f0
#define FUN_004b4120 PortTop_Bound_004b4120
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c3d60 RwCore_004c3d60   // exe body (RwCore/RwCore_004c3d60.cpp)
extern "C" {
#include "../../../../re/match/s14/0048a4e0.c"
}
#endif // MASHED_STANDALONE
