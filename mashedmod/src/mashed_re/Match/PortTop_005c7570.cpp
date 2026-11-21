// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x005c7570 = re/match/s28/005c7570.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005c7570 PortTop_Bound_005c7570
#define FUN_005ab040 PortTop_Bound_005ab040   // exe body (Match/PortTop_005ab040.cpp)
extern "C" {
#include "../../../../re/match/s28/005c7570.c"
}
extern "C" { void *PortTop_Keep_005c7570 = (void *)PortTop_Bound_005c7570; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
