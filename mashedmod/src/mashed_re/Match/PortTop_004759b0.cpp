// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x004759b0 = the matched source re/match/s12/004759b0.c
// (MATCH-S12, BYTE-IDENTICAL): DAT_00691600 = param_1, then DAT_00691608 = 0x00474db0(param_1, 0x40) and DAT_00691604 /
// DAT_00691610 / DAT_00691614 = 0x00474db0(param_1, 4). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it with 2
// on the MASHED_ORIG_TOP path (WinMain 0x00492370 -> 0x00402750 -> 0x0040bbb0).
// Callee binds by name, no raw-address call: 0x00474db0 -> Orig_00474db0 (Match/PortTop_00474db0.cpp: RwGlobals +0x108
// librw malloc + 0x004b6520 zero fill).
// Shared state: no other exe TU reads or writes 0x00691600..0x00691617.
// The two records are filled by 0x00475a00 (Match/PortTop_00475a00.cpp, an L-115 near-match body since PORT-OT-RENDER-2),
// and the teardown 0x00475d90 (Match/PortTop_00475d90.cpp) frees them and the four blocks at AppDestroy.
// The other body of this RVA is the .asi-only Match/MatchS12_004759b0.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004759b0 Orig_004759b0
#define FUN_00474db0 Orig_00474db0                              // matched, Match/PortTop_00474db0.cpp
extern "C" {
#include "../../../../re/match/s12/004759b0.c"
}
#endif  // MASHED_STANDALONE
