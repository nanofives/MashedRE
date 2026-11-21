// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00411750 = re/match/s02/00411750.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411750 PortTop_Bound_00411750
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00482860 PortTop_BulkStop_00482860   // named stop
#define FUN_00483a30 PortTop_BulkStop_00483a30   // named stop
extern "C" {
#include "../../../../re/match/s02/00411750.c"
}
extern "C" { void *PortTop_Keep_00411750 = (void *)PortTop_Bound_00411750; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
