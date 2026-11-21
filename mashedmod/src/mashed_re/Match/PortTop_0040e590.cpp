// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040e590 = re/match/t01g/0040e590.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040e590 PortTop_Bound_0040e590
#define FUN_00408a70 PortTop_Bound_00408a70
#define FUN_00408b00 PortTop_RFStop_00408b00   // named stop
#define FUN_00409290 PortTop_Bound_00409290
#define FUN_0040b250 UtilReset8a9500   // exe body (Match/MatchS02_0040b250.cpp)
#define FUN_0040b410 PortTop_Bound_0040b410
#define FUN_0040e3a0 PortTop_Bound_0040e3a0
#define FUN_0041ede0 PortTop_Bound_0041ede0
#define FUN_0041ee50 PortTop_Bound_0041ee50
#define FUN_0041ef80 PortTop_Bound_0041ef80
#define FUN_0041f000 TimerSlotDataCopy   // exe body (Match/MatchS04_0041f000.cpp)
#define FUN_00429820 Clear8991b0Pair   // exe body (Match/MatchS06_00429820.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_00430790 GetDat0067f17c   // exe body (Match/MatchS07_00430790.cpp)
#define FUN_0045ba00 RaceResultIndexedStore   // exe body (Match/MatchS10_0045ba00.cpp)
#define FUN_0046b1c0 PortTop_RFStop_0046b1c0   // named stop
#define FUN_0046b540 PortTop_RFStop_0046b540   // named stop
#define FUN_0046baa0 VehiclePhysicsStateReset   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c6d0 VehicleEntitySlotRead   // exe body (Match/MatchS11_0046c6d0.cpp)
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_004704c0 PortTop_Bound_004704c0
#define FUN_0048f680 PortTop_Bound_0048f680
#define FUN_0048f740 PortTop_Bound_0048f740
extern "C" {
#include "../../../../re/match/t01g/0040e590.c"
}
#endif // MASHED_STANDALONE
