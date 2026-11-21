// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040b090 = re/match/s02/0040b090.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040b090 PortTop_Bound_0040b090
#define FUN_0040ad40 PortTop_Bound_0040ad40
#define FUN_0040ad90 PortTop_Bound_0040ad90
#define FUN_0040ae30 PortTop_Bound_0040ae30
#define FUN_0040aef0 PortTop_RFStop_0040aef0   // named stop
#define FUN_004292c0 UtilWritePtr   // exe body (Match/MatchS06_004292c0.cpp)
#define FUN_004292d0 FUN_004292d0   // exe body (Match/MatchS06_004292d0.cpp)
#define FUN_0046d320 PortTop_Bound_0046d320
#define FUN_0046d360 PortTop_Bound_0046d360
#define FUN_0055dec0 UtilDeref55dec0   // exe body (Util/UtilLeaves_ab6.cpp)
extern "C" {
#include "../../../../re/match/s02/0040b090.c"
}
#endif // MASHED_STANDALONE
