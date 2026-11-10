// PORT-OT-RENDER-2 (L-111 step 1): exe-only port body of rva 0x00475d90 = the matched source re/match/s12/00475d90.c
// (MATCH-S12): for each of the DAT_00691600 records, 0x004768c0 on the 0x40-byte record DAT_00691608 + i * 0x40 and the
// RwGlobals +0x10c free of DAT_00691604[i] when non-zero; then RwGlobals +0x10c on the four arrays DAT_00691608 / 04 / 10 /
// 14 and all four set to 0. The matched 0x0040bd00 (Match/PortTop_0040bd00.cpp) calls it on the MASHED_ORIG_TOP path
// (AppDestroy 0x00402a40 -> 0x0040bd00).
// Builders of what it reads (the teardown lesson): the four arrays and DAT_00691600 = 2 come from the matched 0x004759b0
// (Match/PortTop_004759b0.cpp), both records and DAT_00691604[0..1] from 0x00475a00 (Match/PortTop_00475a00.cpp, the L-115
// near-match body), each called from the matched 0x0040bbb0 on the same path. 0x004768c0 reads *(record + 4) with no null
// check, so this body runs only with 0x00475a00 swapped. The record +0x04 is the RW-layout PTank record that 0x004770c0
// makes (RpPTankAtomicCreate binding, LibRw/RwApiBind.cpp), the arrays are librw mallocs (RwGlobals +0x108 in 0x00474db0).
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp).
// Shared state: no other exe TU reads or writes 0x00691600..0x00691617.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00475d90 Orig_00475d90
#define FUN_004768c0 Orig_004768c0                              // matched, Match/PortTop_004768c0.cpp
extern "C" {
#include "../../../../re/match/s12/00475d90.c"
}
#endif  // MASHED_STANDALONE
