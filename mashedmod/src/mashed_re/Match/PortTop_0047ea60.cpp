// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0047ea60 = re/match/s13/0047ea60.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047ea60 PortTop_Bound_0047ea60
#define FUN_004c0e50 RwCore_004c0e50   // exe body (RwCore/RwCore_004c0e50.cpp)
#define FUN_004c3df0 RwV3dTransformPoints   // exe body (Match/MatchS19_004c3df0.cpp)
#define FUN_004c51a0 RwCore_004c51a0   // exe body (RwCore/RwCore_004c51a0.cpp)
#define FUN_0055ac00 PortTop_RFStop_0055ac00   // named stop
#define FUN_0055dff0 PortTop_RFStop_0055dff0   // named stop
#define FUN_0057c210 PortTop_RFStop_0057c210   // named stop
extern "C" {
#include "../../../../re/match/s13/0047ea60.c"
}
#endif // MASHED_STANDALONE
