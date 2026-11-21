// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004e6680 = re/match/s21/004e6680.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004e6680 PortTop_Bound_004e6680
#define FUN_004c0ed0 RwCore_004c0ed0   // exe body (RwCore/RwCore_004c0ed0.cpp)
extern "C" {
#include "../../../../re/match/s21/004e6680.c"
}
extern "C" { void *PortTop_Keep_004e6680 = (void *)PortTop_Bound_004e6680; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
