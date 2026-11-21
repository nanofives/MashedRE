// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00420230 = re/match/s05/00420230.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00420230 PortTop_Bound_00420230
#define FUN_0041f330 PortTop_Local_00420230_0041f330   // file-local in this source
extern "C" {
#include "../../../../re/match/s05/00420230.c"
}
extern "C" { void *PortTop_Keep_00420230 = (void *)PortTop_Bound_00420230; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
