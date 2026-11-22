// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040d470 = re/match/s02/0040d470.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040d470 PortTop_Bound_0040d470
#define FUN_00405430 PortTop_RFStop_00405430   // named stop
#define FUN_004414b0 PortTop_RFStop_004414b0   // named stop
#define FUN_00448220 PortTop_Bound_00448220   // exe body (Match/PortTop_00448220.cpp)
#define FUN_004671a0 RwCore_004671a0   // exe body (RwCore/RwCore_004671a0.cpp)
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
extern "C" {
#include "../../../../re/match/s02/0040d470.c"
}
#endif // MASHED_STANDALONE
