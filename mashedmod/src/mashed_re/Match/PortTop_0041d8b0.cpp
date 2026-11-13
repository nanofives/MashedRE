// PORT-TOP-04K (L-96 top-down lane): exe-only port body of rva 0x0041d8b0 = the matched source re/match/s04/0041d8b0.c
// (BYTE-IDENTICAL 96 B): startlights.dff -> DAT_0063d57c, its frame -> DAT_0063d580, the 6 atomics at 0x0063d55c filled
// by part code (0x004b51d0), each given geometry flag 0x40 (0x004b52f0). Caller: the matched 0x00402750 on the
// MASHED_ORIG_TOP path (its 04E stand-in PortTop_StandIn04E_0041d8b0 is gone). Teardown: the matched 0x0041da80
// (RpClumpDestroy(DAT_0063d57c), Match/PortTop_0041da80.cpp) from AppDestroy.
// Callees bind by name (extern "C", cdecl, every argument a 4-byte word), no raw-address call:
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body; a librw rw::Clump)
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp)
//  - 0x004b51d0 = Orig_004b51d0 (Match/PortTop_004b51d0.cpp)
//  - 0x004b52f0 = MashedRw_AtomicGeometryFlags_004b52f0 (LibRw/RwApiBind.cpp, takes the librw atomics 0x004b51d0 stores)
// No new RW binding (L-124). The clump's frame is read at clump +0x04: librw Clump +0x04 is object.parent (asserted in
// LibRw/RwApiBind.cpp, PORT-TOP-04H).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0041d8b0 Orig_0041d8b0
#define FUN_004b6520 Orig_004b6520
#define FUN_004b51d0 Orig_004b51d0
#define FUN_004b52f0 MashedRw_AtomicGeometryFlags_004b52f0
extern "C" {
#include "../../../../re/match/s04/0041d8b0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
