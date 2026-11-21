// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00496530 = re/match/s16/004967e0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496530 PortTop_Bound_00496530
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_004957a0 PortTop_BulkStop_004957a0   // named stop
#define FUN_00495fe0 PortTop_Bound_00495fe0
#define FUN_00496100 PortTop_Bound_00496100
#define FUN_004972b0 PortTop_Bound_004972b0
#define FUN_00497310 PortTop_Bound_00497310   // exe body (Match/PortTop_00497310.cpp)
#define FUN_00497450 Pred497450   // exe body (Match/MatchGFFU_00497450.cpp)
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_004967e0 PortTop_Local_00496530_004967e0   // file-local in this source
extern "C" {
#include "../../../../re/match/s16/004967e0.c"
}
extern "C" { void *PortTop_Keep_00496530 = (void *)PortTop_Bound_00496530; }   // anchor: BulkRefs /include (the source defines it static)
#endif // MASHED_STANDALONE
