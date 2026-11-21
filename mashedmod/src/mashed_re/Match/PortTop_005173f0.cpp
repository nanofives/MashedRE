// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x005173f0 = re/match/s23/005173f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005173f0 PortTop_Bound_005173f0
#define FUN_00516cc0 PortTop_BulkStop_00516cc0   // named stop
#define FUN_00516d20 PortTop_Bound_00516d20
#define FUN_00517250 PortTop_BulkStop_00517250   // named stop
#define FUN_005173d0 PortTop_BulkStop_005173d0   // named stop
#define FUN_0051ca10 PortTop_Bound_0051ca10
#define FUN_005207c0 PortTop_BulkStop_005207c0   // named stop
#define FUN_00520850 PortTop_BulkStop_00520850   // named stop
#define FUN_005208c0 PortTop_BulkStop_005208c0   // named stop
#define FUN_00520930 PortTop_BulkStop_00520930   // named stop
#define FUN_005209d0 PortTop_Bound_005209d0
#define FUN_00522cc0 PortTop_BulkStop_00522cc0   // named stop
extern "C" {
#include "../../../../re/match/s23/005173f0.c"
}
extern "C" { void *PortTop_Keep_005173f0 = (void *)PortTop_Bound_005173f0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
