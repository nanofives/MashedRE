// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004a787f = re/match/s17/004a787f.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004a787f PortTop_Bound_004a787f
#define FUN_004a4b6e PortTop_BulkStop_004a4b6e   // named stop
#define FUN_004a7800 PortTop_BulkStop_004a7800   // named stop
extern "C" {
#include "../../../../re/match/s17/004a787f.c"
}
extern "C" { void *PortTop_Keep_004a787f = (void *)PortTop_Bound_004a787f; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
