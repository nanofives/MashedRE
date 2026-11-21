// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x005401d0 = re/match/rwl03/005401d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005401d0 PortTop_Bound_005401d0
#define FUN_004c5a60 RwCore_004c5a60   // exe body (RwCore/RwCore_004c5a60.cpp)
#define FUN_004d8560 UtilReturnOne4d8560   // exe body (Util/UtilLeaves_ab6.cpp)
extern "C" {
#include "../../../../re/match/rwl03/005401d0.c"
}
extern "C" { void *PortTop_Keep_005401d0 = (void *)PortTop_Bound_005401d0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
