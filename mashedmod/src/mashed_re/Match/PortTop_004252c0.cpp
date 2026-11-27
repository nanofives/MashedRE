// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x004252c0 = re/match/s05/004252c0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004252c0 PortTop_Bound_004252c0
#define FUN_00407a40 PortTop_RFStop_00407a40   // named stop
#define FUN_0040e350 GetRenderSubMode   // exe body (Match/MatchS02_0040e350.cpp)
#define FUN_0040e370 IsCarSlotActive   // exe body (Match/MatchS02_0040e370.cpp)
#define FUN_0040e470 CarSlotStateGet   // exe body (Match/MatchS02_0040e470.cpp)
#define FUN_00415860 PortTop_RFStop_00415860   // named stop
#define FUN_004233e0 HeadingAtan2ToGameAngle   // exe body (Ai/HeadingAtan2.cpp)
#define FUN_00426cc0 VehicleTable4cPtrGet   // exe body (Match/MatchS06_00426cc0.cpp)
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_0046cbb0 PortTop_RFStop_0046cbb0   // named stop
#define FUN_0046cc10 VehicleFloatFieldAsInt   // exe body (Match/MatchS11_0046cc10.cpp)
#define FUN_0046d510 PortTop_Bound_0046d510
#define FUN_004a3384 MashedCrt_acos_004a3384   // exe body (Match/PortTop_CrtMath.cpp)
extern "C" {
#include "../../../../re/match/s05/004252c0.c"
}
#endif // MASHED_STANDALONE
