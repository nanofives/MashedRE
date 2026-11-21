// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x0041fe10 = re/match/s05/0041fe10.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041fe10 PortTop_Bound_0041fe10
#define FUN_00420420 PortTop_BulkStop_00420420   // named stop
#define FUN_004223b0 PortTop_Bound_004223b0
#define FUN_004b3fc0 Orig_004b3fc0   // exe body (Match/PortTop_004b3fc0.cpp)
#define FUN_004b5190 RwCore_004b5190   // exe body (RwCore/RwCore_004b5190.cpp)
#define FUN_004e5fc0 RwCore_004e5fc0   // exe body (RwCore/RwCore_004e5fc0.cpp)
#define FUN_004e6920 RwCore_004e6920   // exe body (RwCore/RwCore_004e6920.cpp)
#define FUN_004e6ab0 RwCore_004e6ab0   // exe body (RwCore/RwCore_004e6ab0.cpp)
#define FUN_004e6e00 RwCore_004e6e00   // exe body (RwCore/RwCore_004e6e00.cpp)
#define FUN_004e6fb0 PortTop_Bound_004e6fb0
extern "C" {
#include "../../../../re/match/s05/0041fe10.c"
}
extern "C" { void *PortTop_Keep_0041fe10 = (void *)PortTop_Bound_0041fe10; }   // anchor: BulkRefs /include (the source defines it static)
#endif // MASHED_STANDALONE
