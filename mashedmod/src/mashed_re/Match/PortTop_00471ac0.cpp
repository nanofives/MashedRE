// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00471ac0 = re/match/s11/00471ac0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00471ac0 PortTop_Bound_00471ac0
#define FUN_00407a20 GetAiLapCounter   // exe body (Match/MatchS01_00407a20.cpp)
#define FUN_0040e180 PortTop_RFStop_0040e180   // named stop
#define FUN_00426020 GlobalDat00646e58Get   // exe body (Match/MatchS05_00426020.cpp)
#define FUN_0047ce40 PortTop_Bound_0047ce40   // exe body (Match/PortTop_0047ce40.cpp)
#define FUN_0047d080 PortTop_Bound_0047d080   // exe body (Match/PortTop_0047d080.cpp)
#define FUN_0047d100 PortTop_Bound_0047d100   // exe body (Match/PortTop_0047d100.cpp)
extern "C" {
#include "../../../../re/match/s11/00471ac0.c"
}
#endif // MASHED_STANDALONE
