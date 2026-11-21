// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00401570 = re/match/s01/00401570.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00401570 PortTop_Bound_00401570
#define FUN_00401da0 PortTop_RFStop_00401da0   // named stop
#define FUN_004e6680 PortTop_Bound_004e6680   // exe body (Match/PortTop_004e6680.cpp)
#define FUN_00401ee0 PortTop_Local_00401570_00401ee0   // file-local in this source
// PORT-TOP-06: MASHED_RE_PORT drops the source extern block (same as Match/MatchS01.cpp:42 / :77-78); the table and
// the current-entry pointer are read at their original addresses.
#define MASHED_RE_PORT 1
#define DAT_00636578 ((Entry636578*)0x00636578u)
#define DAT_00636ac0 (*(Entry636578**)0x00636ac0u)
extern "C" {
void FUN_00401da0(void);   // PORT-TOP-06: re/match/s01/00401570.c declares these two only outside MASHED_RE_PORT
void FUN_004e6680(void *e);   // the source passes Entry636578 * (typedef declared later in the source)
#include "../../../../re/match/s01/00401570.c"
}
#endif // MASHED_STANDALONE
