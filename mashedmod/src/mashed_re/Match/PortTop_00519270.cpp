// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00519270 = re/match/s23/00519270.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00519270 PortTop_Bound_00519270
#define FUN_00515710 PortTop_BulkStop_00515710   // named stop
#define FUN_00515740 PortTop_BulkStop_00515740   // named stop
#define FUN_00515770 PortTop_BulkStop_00515770   // named stop
#define FUN_005157c0 PortTop_BulkStop_005157c0   // named stop
#define FUN_005159d0 PortTop_BulkStop_005159d0   // named stop
#define FUN_00517250 PortTop_BulkStop_00517250   // named stop
#define FUN_00517270 PortTop_BulkStop_00517270   // named stop
#define FUN_005196f0 PortTop_BulkStop_005196f0   // named stop
#define FUN_00519820 PortTop_BulkStop_00519820   // named stop
#define FUN_005199f0 PortTop_BulkStop_005199f0   // named stop
#define FUN_00519af0 PortTop_BulkStop_00519af0   // named stop
#define FUN_00519bc0 PortTop_BulkStop_00519bc0   // named stop
#define FUN_00519e30 PortTop_BulkStop_00519e30   // named stop
#define FUN_00519f70 PortTop_BulkStop_00519f70   // named stop
#define FUN_0051a6c0 PortTop_BulkStop_0051a6c0   // named stop
#define FUN_0051b750 PortTop_BulkStop_0051b750   // named stop
#define FUN_0051bb40 PortTop_BulkStop_0051bb40   // named stop
#define FUN_0051bd70 PortTop_BulkStop_0051bd70   // named stop
#define FUN_0051c100 PortTop_BulkStop_0051c100   // named stop
extern "C" {
#include "../../../../re/match/s23/00519270.c"
}
extern "C" { void *PortTop_Keep_00519270 = (void *)PortTop_Bound_00519270; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
