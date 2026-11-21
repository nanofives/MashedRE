// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0044e0a0 = re/match/s08/0044e0a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0044e0a0 PortTop_Bound_0044e0a0
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_00480720 PortTop_Bound_00480720
#define FUN_00481750 PortTop_Bound_00481750
#define FUN_0048aa20 PortTop_Bound_0048aa20
#define FUN_004b4510 PortTop_Bound_004b4510
#define FUN_004b4550 PortTop_RFStop_004b4550   // named stop
extern "C" {
#include "../../../../re/match/s08/0044e0a0.c"
}
#endif // MASHED_STANDALONE
