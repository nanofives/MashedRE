// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00517940 = re/match/s23/00517940.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00517940 PortTop_Bound_00517940
#define FUN_00516d30 PortTop_BulkStop_00516d30   // named stop
#define FUN_00517250 PortTop_BulkStop_00517250   // named stop
#define FUN_00519270 PortTop_Bound_00519270
#define FUN_0051c9f0 PortTop_BulkStop_0051c9f0   // named stop
#define FUN_00520960 PortTop_BulkStop_00520960   // named stop
#define FUN_00522ce0 PortTop_BulkStop_00522ce0   // named stop
#define FUN_00523110 PortTop_BulkStop_00523110   // named stop
#define FUN_00523140 PortTop_BulkStop_00523140   // named stop
#define FUN_00523170 PortTop_Bound_00523170
#define FUN_00527ee0 PortTop_Bound_00527ee0
#define FUN_00528200 PortTop_BulkStop_00528200   // named stop
#define FUN_00528630 PortTop_BulkStop_00528630   // named stop
#define FUN_00528810 PortTop_Bound_00528810
#define FUN_00528ba0 PortTop_BulkStop_00528ba0   // named stop
extern "C" {
#include "../../../../re/match/s23/00517940.c"
}
extern "C" { void *PortTop_Keep_00517940 = (void *)PortTop_Bound_00517940; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
