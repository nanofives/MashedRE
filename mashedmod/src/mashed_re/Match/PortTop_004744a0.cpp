// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x004744a0 = re/match/s11/004744a0.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004744a0 PortTop_Bound_004744a0
#define FUN_0042b8b0 ScreenWidthGet   // exe body (Match/MatchS07_0042b8b0.cpp)
#define FUN_0042b8c0 ScreenHeightGet   // exe body (Match/MatchS07_0042b8c0.cpp)
extern "C" {
#include "../../../../re/match/s11/004744a0.c"
}
extern "C" { void *PortTop_Keep_004744a0 = (void *)PortTop_Bound_004744a0; }   // anchor: BulkRefs /include
#endif // MASHED_STANDALONE
