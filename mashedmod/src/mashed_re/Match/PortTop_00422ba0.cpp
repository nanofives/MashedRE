// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00422ba0 = re/match/s05/00422ba0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00422ba0 PortTop_Bound_00422ba0
#define FUN_0040e470 CarSlotStateGet   // exe body (Match/MatchS02_0040e470.cpp)
#define FUN_00417370 PortTop_RFStop_00417370   // named stop
#define FUN_004219f0 PortTop_Bound_004219f0
#define FUN_0044e0a0 PortTop_Bound_0044e0a0
#define FUN_0046cbb0 PortTop_RFStop_0046cbb0   // named stop
#define FUN_0046cbe0 VehicleCarStateSet   // exe body (Match/MatchS11_0046cbe0.cpp)
#define FUN_0047cea0 PortTop_Bound_0047cea0
#define FUN_00480720 PortTop_Bound_00480720
#define FUN_00480d60 PortTop_RFStop_00480d60   // named stop
#define FUN_004922e0 PortTop_Bound_004922e0
#define FUN_004c3ac0 RwCore_004c3ac0   // exe body (RwCore/RwCore_004c3ac0.cpp)
extern "C" {
#include "../../../../re/match/s05/00422ba0.c"
}
#endif // MASHED_STANDALONE
