// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00411d90 = re/match/topm01/00411d90.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00411d90 PortTop_Bound_00411d90
#define FUN_00411350 PortTop_Bound_00411350   // exe body (Match/PortTop_00411350.cpp)
#define FUN_00411750 PortTop_Bound_00411750
#define FUN_00426c00 GameStateFlagGet   // exe body (Match/MatchS06_00426c00.cpp)
#define FUN_004299d0 PortTop_BulkStop_004299d0   // named stop
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00430820 PortTop_Bound_00430820   // exe body (Match/PortTop_00430820.cpp)
#define FUN_00482900 PortTop_BulkStop_00482900   // named stop
#define FUN_00482930 PortTop_BulkStop_00482930   // named stop
#define FUN_00483d10 PortTop_BulkStop_00483d10   // named stop
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
#define FUN_004a2cbd PortTop_Crt_004a2cbd   // exe body (Match/PortTop_00493710.cpp)
extern "C" {
#include "../../../../re/match/topm01/00411d90.c"
}
extern "C" { void *PortTop_Keep_00411d90 = (void *)PortTop_Bound_00411d90; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
