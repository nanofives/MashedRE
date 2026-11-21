// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x00421a90 = re/match/s05/004220d0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00421a90 PortTop_Bound_00421a90
#define FUN_004218d0 PortTop_Bound_004218d0   // exe body (Match/PortTop_004218b0.cpp)
#define FUN_00421930 BodyGeometryFirstDword   // exe body (Match/MatchS05_00421930.cpp)
#define FUN_0042a5d0 PortTop_Bound_0042a5d0   // exe body (Match/PortTop_0042a5d0.cpp)
#define FUN_004b3fc0 Orig_004b3fc0   // exe body (Match/PortTop_004b3fc0.cpp)
#define FUN_004b5190 RwCore_004b5190   // exe body (RwCore/RwCore_004b5190.cpp)
#define FUN_004b6520 Orig_004b6520   // exe body (Match/PortTop_004b6520.cpp)
#define FUN_004c1210 RwCore_004c1210   // exe body (RwCore/RwCore_004c1210.cpp)
#define FUN_004e5fc0 RwCore_004e5fc0   // exe body (RwCore/RwCore_004e5fc0.cpp)
#define FUN_0055dff0 PortTop_RLStop_0055dff0   // named stop
#define FUN_00421ba0 PortTop_Local_00421a90_00421ba0   // file-local in this source
#define FUN_004220d0 PortTop_Local_00421a90_004220d0   // file-local in this source
#define FUN_00421ba0 PortTop_Bound_00421ba0   // member of this source, its only exe copy
#define FUN_004220d0 PortTop_Bound_004220d0   // member of this source, its only exe copy
extern "C" {
#include "../../../../re/match/s05/004220d0.c"
}
extern "C" { void *PortTop_Keep_00421a90 = (void *)PortTop_Bound_00421a90; }   // anchor: BulkRefs /include (the source defines it static)
extern "C" { void *PortTop_Keep_00421ba0 = (void *)PortTop_Bound_00421ba0; }   // anchor: BulkRefs /include (the source defines it static)
extern "C" { void *PortTop_Keep_004220d0 = (void *)PortTop_Bound_004220d0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
