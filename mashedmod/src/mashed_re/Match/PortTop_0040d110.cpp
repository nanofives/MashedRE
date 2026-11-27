// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x0040d110 = re/match/topm01/0040d110.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040d110 PortTop_Bound_0040d110
#define FUN_0040cea0 PortTop_Bound_0040cea0   // exe body (Match/PortTop_0040cea0.cpp)
#define FUN_0041a8d0 PortTop_BulkStop_0041a8d0   // named stop
#define FUN_00420230 PortTop_Bound_00420230
#define FUN_00420420 PortTop_Bound_00420420
#define FUN_004220d0 PortTop_Bound_004220d0   // exe body (Match/PortTop_00421a90.cpp)
#define FUN_0042a8d0 PortTop_Bound_0042a8d0   // exe body (Match/PortTop_0042a8d0.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00431d70 PortTop_Bound_00431d70
#define FUN_0045b350 RwInitNullStub   // exe body (Match/MatchS09_0045b350.cpp)
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
extern "C" {
#include "../../../../re/match/topm01/0040d110.c"
}
extern "C" { void *PortTop_Keep_0040d110 = (void *)PortTop_Bound_0040d110; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
