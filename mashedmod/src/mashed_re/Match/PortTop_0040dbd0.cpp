// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x0040dbd0 = re/match/s02/0040dbd0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040dbd0 PortTop_Bound_0040dbd0
#define FUN_0041b520 Update41b520   // exe body (Race/SubStateCallees.cpp)
#define FUN_0041d910 Clear63d584Pair   // exe body (Match/MatchS04_0041d910.cpp)
#define FUN_0041e080 ScoreboardStateZeroInit   // exe body (Match/MatchS04_0041e080.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
extern "C" {
#include "../../../../re/match/s02/0040dbd0.c"
}
#endif // MASHED_STANDALONE
