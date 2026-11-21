// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00496100 = re/match/s16/00496100.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496100 PortTop_Bound_00496100
#define FUN_004b6480 Orig_004b6480   // exe body (Match/PortTop_004b6480.cpp)
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
extern "C" {
#include "../../../../re/match/s16/00496100.c"
}
extern "C" { void *PortTop_Keep_00496100 = (void *)PortTop_Bound_00496100; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
