// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0047c160 = re/match/s13/0047c160.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0047c160 PortTop_Bound_0047c160
#define FUN_0047bb10 PortTop_RFStop_0047bb10   // named stop
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
#define FUN_004c1b40 PortTop_Bound_004c1b40
extern "C" {
#include "../../../../re/match/s13/0047c160.c"
}
#endif // MASHED_STANDALONE
