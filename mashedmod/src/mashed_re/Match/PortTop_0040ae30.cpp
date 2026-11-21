// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040ae30 = re/match/s02/0040ae30.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040ae30 PortTop_Bound_0040ae30
#define FUN_0046bf50 PortTop_Bound_0046bf50
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
#define FUN_0046d660 PortTop_Bound_0046d660
#define FUN_004c3bf0 RwCore_004c3bf0   // exe body (RwCore/RwCore_004c3bf0.cpp)
extern "C" {
#include "../../../../re/match/s02/0040ae30.c"
}
#endif // MASHED_STANDALONE
