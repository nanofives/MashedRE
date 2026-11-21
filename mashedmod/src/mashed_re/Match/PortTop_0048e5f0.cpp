// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0048e5f0 = re/match/s15/0048e5f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048e5f0 PortTop_Bound_0048e5f0
#define FUN_00467210 PortTop_Bound_00467210
#define FUN_00476df0 PortTop_Bound_00476df0
#define FUN_0048d140 PortTop_RFStop_0048d140   // named stop
#define FUN_0048d360 PortTop_Bound_0048d360
#define FUN_0048d540 PortTop_Bound_0048d540
#define FUN_0048d720 PortTop_RFStop_0048d720   // named stop
#define FUN_0048d900 PortTop_RFStop_0048d900   // named stop
#define FUN_0048db20 PortTop_RFStop_0048db20   // named stop
#define FUN_0048dd40 PortTop_RFStop_0048dd40   // named stop
#define FUN_0048df60 PortTop_RFStop_0048df60   // named stop
#define FUN_0048e180 PortTop_RFStop_0048e180   // named stop
#define FUN_0048e360 PortTop_RFStop_0048e360   // named stop
#define FUN_004b6480 Orig_004b6480   // exe body (Match/PortTop_004b6480.cpp)
extern "C" {
#include "../../../../re/match/s15/0048e5f0.c"
}
#endif // MASHED_STANDALONE
