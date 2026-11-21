// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00522b50 = re/match/s23/00522b50.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00522b50 PortTop_Bound_00522b50
#define FUN_00528e00 PortTop_BulkStop_00528e00   // named stop
#define FUN_00528e20 PortTop_BulkStop_00528e20   // named stop
#define FUN_0052a890 PortTop_BulkStop_0052a890   // named stop
#define FUN_0052a9c0 PortTop_BulkStop_0052a9c0   // named stop
#define FUN_0052aa40 PortTop_BulkStop_0052aa40   // named stop
#define FUN_0052b750 PortTop_Bound_0052b750
extern "C" {
#include "../../../../re/match/s23/00522b50.c"
}
extern "C" { void *PortTop_Keep_00522b50 = (void *)PortTop_Bound_00522b50; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
