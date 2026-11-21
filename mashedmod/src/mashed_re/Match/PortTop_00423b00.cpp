// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00423b00 = re/match/s05/00423b00.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00423b00 PortTop_Bound_00423b00
#define FUN_00423040 PortTop_Bound_00423040
#define FUN_00423270 TabCycler   // exe body (Match/MatchS05_00423270.cpp)
#define FUN_00423320 CursorMover   // exe body (Match/MatchS05_00423320.cpp)
#define FUN_00423670 PortTop_Bound_00423670
extern "C" {
#include "../../../../re/match/s05/00423b00.c"
}
#endif // MASHED_STANDALONE
