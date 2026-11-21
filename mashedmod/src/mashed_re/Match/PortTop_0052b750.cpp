// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x0052b750 = re/match/s23/0052b750.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0052b750 PortTop_Bound_0052b750
#define FUN_0052bf20 PortTop_BulkStop_0052bf20   // named stop
extern "C" {
#include "../../../../re/match/s23/0052b750.c"
}
extern "C" { void *PortTop_Keep_0052b750 = (void *)PortTop_Bound_0052b750; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
