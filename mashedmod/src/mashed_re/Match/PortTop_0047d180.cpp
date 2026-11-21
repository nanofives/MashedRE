// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0047d180 = re/match/s13/0047d180.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047d180 PortTop_Bound_0047d180
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#define FUN_004c45f0 RwCore_004c45f0   // exe body (RwCore/RwCore_004c45f0.cpp)
#define FUN_004c52f0 PortTop_RFStop_004c52f0   // named stop
#define FUN_0055ab30 PortTop_RFStop_0055ab30   // named stop
#define FUN_0055ac00 PortTop_RFStop_0055ac00   // named stop
#define FUN_0055ade0 PortTop_Bound_0055ade0   // exe body (Match/PortTop_0055ade0.cpp)
#define FUN_0055dff0 PortTop_RFStop_0055dff0   // named stop
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
#define FUN_0057c420 PortTop_Bound_0057c420   // exe body (Match/PortTop_0057c420.cpp)
extern "C" {
#include "../../../../re/match/s13/0047d180.c"
}
#endif // MASHED_STANDALONE
