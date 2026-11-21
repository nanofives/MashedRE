// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004074a0 = re/match/s01/004074a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004074a0 PortTop_Bound_004074a0
#define FUN_0047f290 PortTop_BulkStop_0047f290   // named stop
#define FUN_00489c60 Orig_00489c60   // exe body (Match/PortTop_00489c60.cpp)
#define FUN_004e6e00 RwCore_004e6e00   // exe body (RwCore/RwCore_004e6e00.cpp)
extern "C" {
#include "../../../../re/match/s01/004074a0.c"
}
extern "C" { void *PortTop_Keep_004074a0 = (void *)PortTop_Bound_004074a0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
