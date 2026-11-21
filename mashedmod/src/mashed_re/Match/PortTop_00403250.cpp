// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00403250 = re/match/s01/00403250.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00403250 PortTop_Bound_00403250
#define FUN_0042b920 ConstantGetter22   // exe body (Match/MatchS07_0042b920.cpp)
#define FUN_00433240 PortTop_Bound_00433240
#define FUN_00443080 PortTop_Bound_00443080
#define FUN_00448700 PortTop_Bound_00448700
#define FUN_0045bed0 PortTop_RFStop_0045bed0   // named stop
#define FUN_0045bf30 Orig_0045bf30   // exe body (Match/PortTop_0045bae0.cpp)
#define FUN_00493590 PortTop_Bound_00493590
extern "C" {
#include "../../../../re/match/s01/00403250.c"
}
#endif // MASHED_STANDALONE
