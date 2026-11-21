// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00481a30 = re/match/s13/00481a30.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00481a30 PortTop_Bound_00481a30
#define FUN_00407670 PortTop_RFStop_00407670   // named stop
#define FUN_004260c0 PortTop_Bound_004260c0   // exe body (Match/PortTop_004260c0.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_00467210 PortTop_Bound_00467210
#define FUN_004726f0 PortTop_Bound_004726f0
#define FUN_0047cd50 PortTop_RFStop_0047cd50   // named stop
#define FUN_004818d0 PortTop_Bound_004818d0
#define FUN_00484cf0 PortTop_Bound_00484cf0
#define FUN_00487280 PortTop_RFStop_00487280   // named stop
#define FUN_00487d00 PortTop_RFStop_00487d00   // named stop
#define FUN_00487d60 PortTop_RFStop_00487d60   // named stop
#define FUN_004a3384 PortTop_RFStop_004a3384   // named stop
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c1b40 PortTop_Bound_004c1b40
#define FUN_004e6100 RwCore_004e6100   // exe body (RwCore/RwCore_004e6100.cpp)
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/00481a30.c"
}
#endif // MASHED_STANDALONE
