// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00411600 = re/match/s02/00411600.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411600 PortTop_Bound_00411600
#define FUN_00407a00 ReadAiCheckpointField   // exe body (Gameplay/RangeTable_ah1.cpp)
#define FUN_00411350 PortTop_Bound_00411350   // exe body (Match/PortTop_00411350.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_004829d0 PortTop_Bound_004829d0
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)
extern "C" {
#include "../../../../re/match/s02/00411600.c"
}
#endif // MASHED_STANDALONE
