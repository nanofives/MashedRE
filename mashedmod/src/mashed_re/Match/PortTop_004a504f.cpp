// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004a504f = re/match/s17/004a504f.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004a504f PortTop_Bound_004a504f
#define FUN_004a45fb MashedCrt_malloc_004a45fb   // exe body (Match/PortTop_CrtHeap.cpp)
#define FUN_004a460d MashedCrt_free_004a460d   // exe body (Match/PortTop_CrtHeap.cpp)
#define FUN_004a4ea8 PortTop_BulkStop_004a4ea8   // named stop
#define FUN_004a9410 PortTop_BulkStop_004a9410   // named stop
#define FUN_004ac435 PortTop_BulkStop_004ac435   // named stop
#define FUN_004a4fc1 PortTop_Local_004a504f_004a4fc1   // file-local in this source
#define FUN_004a4ff4 PortTop_Local_004a504f_004a4ff4   // file-local in this source
#define FUN_004a5018 PortTop_Local_004a504f_004a5018   // file-local in this source
extern "C" {
#include "../../../../re/match/s17/004a504f.c"
}
extern "C" { void *PortTop_Keep_004a504f = (void *)PortTop_Bound_004a504f; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
