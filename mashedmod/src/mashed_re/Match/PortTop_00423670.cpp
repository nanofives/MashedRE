// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00423670 = re/match/s05/00423670.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00423670 PortTop_Bound_00423670
#define FUN_00408b00 PortTop_RFStop_00408b00   // named stop
#define FUN_00423540 PortTop_RFStop_00423540   // named stop
#define FUN_00423630 PortTop_Bound_00423630   // exe body (Match/PortTop_00423630.cpp)
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_004704c0 PortTop_Bound_004704c0
extern "C" {
#include "../../../../re/match/s05/00423670.c"
}
#endif // MASHED_STANDALONE
