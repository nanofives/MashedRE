// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00410860 = re/match/racepath/00410860.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00410860 PortTop_Bound_00410860
#define FUN_00408a50 PerCarRaceProgressGet   // exe body (Match/MatchS01.cpp)
#define FUN_0040b6d0 UtilArrayGet8a94e0   // exe body (Match/MatchS02_0040b6d0.cpp)
#define FUN_0040e460 Flag63b908Set   // exe body (Match/MatchS02_0040e460.cpp)
#define FUN_0041eda0 TimerBitFieldSet   // exe body (Match/MatchS04_0041eda0.cpp)
#define FUN_00424b80 PortTop_RFStop_00424b80   // named stop
#define FUN_00429860 PortTop_Bound_00429860
#define FUN_00429b30 PortTop_Bound_00429b30
#define FUN_0042aab0 UtilInit898a90   // exe body (Match/MatchS06_0042aab0.cpp)
#define FUN_0042f500 GetDat0067ea64   // exe body (Match/MatchS07_0042f500.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_0043aee0 PortTop_Bound_0043aee0
#define FUN_00448700 PortTop_Bound_00448700
#define FUN_0046c7b0 PortTop_RFStop_0046c7b0   // named stop
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
extern "C" {
#include "../../../../re/match/racepath/00410860.c"
}
#endif // MASHED_STANDALONE
