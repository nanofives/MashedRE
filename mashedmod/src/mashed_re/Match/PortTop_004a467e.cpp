// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004a467e = re/match/s17/004a467e.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004a467e PortTop_Bound_004a467e
#define FUN_004a77eb PortTop_Bound_004a77eb
#define FUN_004a787f PortTop_Bound_004a787f
#define FUN_004aac76 PortTop_BulkStop_004aac76   // named stop
#define FUN_004aaf72 PortTop_BulkStop_004aaf72   // named stop
#define FUN_004aaf90 PortTop_BulkStop_004aaf90   // named stop
extern "C" {
#include "../../../../re/match/s17/004a467e.c"
}
extern "C" { void *PortTop_Keep_004a467e = (void *)PortTop_Bound_004a467e; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
