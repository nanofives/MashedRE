// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x0052c7a0 = re/match/s23/0052c7a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0052c7a0 PortTop_Bound_0052c7a0
extern "C" {
#include "../../../../re/match/s23/0052c7a0.c"
}
extern "C" { void *PortTop_Keep_0052c7a0 = (void *)PortTop_Bound_0052c7a0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
