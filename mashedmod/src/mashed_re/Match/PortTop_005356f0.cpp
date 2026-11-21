// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x005356f0 = re/match/rwl03/005356f0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005356f0 PortTop_Bound_005356f0
#define FUN_004e6920 RwCore_004e6920   // exe body (RwCore/RwCore_004e6920.cpp)
#define thunk_FUN_004e6920 PortTop_Bound_005356f0   // the source names this body thunk_FUN_004e6920 (a jump thunk)
extern "C" {
#include "../../../../re/match/rwl03/005356f0.c"
}
extern "C" { void *PortTop_Keep_005356f0 = (void *)PortTop_Bound_005356f0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
