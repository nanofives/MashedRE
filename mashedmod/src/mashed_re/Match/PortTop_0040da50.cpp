// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040da50 = re/match/s02/0040da50.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040da50 PortTop_Bound_0040da50
#define FUN_00408610 PortTop_Bound_00408610
#define FUN_0040d8f0 PortTop_Bound_0040d8f0
#define FUN_0041eeb0 PortTop_Bound_0041eeb0
#define FUN_00420870 PortTop_RFStop_00420870   // named stop
#define FUN_00426080 GlobalDat00656ed8Get   // exe body (Match/MatchS06_00426080.cpp)
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_0046c770 VehicleDestructionStateGetter   // exe body (Match/MatchS11_0046c770.cpp)
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
extern "C" {
#include "../../../../re/match/s02/0040da50.c"
}
#endif // MASHED_STANDALONE
