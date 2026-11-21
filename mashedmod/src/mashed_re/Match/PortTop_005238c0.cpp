// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x005238c0 = re/match/s23/005238c0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005238c0 PortTop_Bound_005238c0
#define FUN_005163a0 PortTop_BulkStop_005163a0   // named stop
#define FUN_00516d50 PortTop_Bound_00516d50
#define FUN_00517250 PortTop_BulkStop_00517250   // named stop
#define FUN_00517270 PortTop_BulkStop_00517270   // named stop
#define FUN_005172b0 PortTop_BulkStop_005172b0   // named stop
#define FUN_00517380 PortTop_BulkStop_00517380   // named stop
#define FUN_0051c9f0 PortTop_BulkStop_0051c9f0   // named stop
#define FUN_00523110 PortTop_BulkStop_00523110   // named stop
#define FUN_005232c0 PortTop_Bound_005232c0
extern "C" {
#include "../../../../re/match/s23/005238c0.c"
}
extern "C" { void *PortTop_Keep_005238c0 = (void *)PortTop_Bound_005238c0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
