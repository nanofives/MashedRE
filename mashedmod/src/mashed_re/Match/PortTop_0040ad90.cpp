// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040ad90 = re/match/s02/0040ad90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040ad90 PortTop_Bound_0040ad90
#define FUN_0046bf50 PortTop_Bound_0046bf50
#define FUN_0046d4a0 PtrCompute881ec8   // exe body (Vehicle/VehicleRecordPtr.cpp)
extern "C" {
#include "../../../../re/match/s02/0040ad90.c"
}
#endif // MASHED_STANDALONE
