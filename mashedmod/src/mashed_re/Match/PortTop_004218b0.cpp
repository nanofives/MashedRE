// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004218b0 = re/match/s05/004218b0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004218b0 PortTop_Bound_004218b0
#define FUN_0055b940 PortTop_RLStop_0055b940   // named stop
#define FUN_0055c4a0 PortTop_Bound_0055c4a0   // exe body (Match/PortTop_0055c4a0.cpp)
#define FUN_0055c4f0 PortTop_Bound_0055c4f0   // exe body (Match/PortTop_0055c4f0.cpp)
#define FUN_0055c540 PortTop_Bound_0055c540   // exe body (Match/PortTop_0055c540.cpp)
#define FUN_0057c220 PortTop_Bound_0057c220   // exe body (Match/PortTop_0057c220.cpp)
#define FUN_0057c300 PortTop_Bound_0057c300   // exe body (Match/PortTop_0057c300.cpp)
#define FUN_0057c500 PortTop_Bound_0057c500   // exe body (Match/PortTop_0057c500.cpp)
#define FUN_0057c550 PortTop_Bound_0057c550   // exe body (Match/PortTop_0057c550.cpp)
#define FUN_004218d0 PortTop_Local_004218b0_004218d0   // file-local in this source
#define FUN_004218d0 PortTop_Bound_004218d0   // member of this source, its only exe copy
extern "C" {
#include "../../../../re/match/s05/004218b0.c"
}
extern "C" { void *PortTop_Keep_004218b0 = (void *)PortTop_Bound_004218b0; }   // anchor: BulkRefs /include (the source defines it static)
extern "C" { void *PortTop_Keep_004218d0 = (void *)PortTop_Bound_004218d0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
