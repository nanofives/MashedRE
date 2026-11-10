// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x00421080 = the matched source re/match/s05/00421080.c
// (BYTE-IDENTICAL 43 B obj symbol): the age field (+0x1c) of the 128 0x2c-byte records at 0x0063e588 set to -1.0f, then
// DAT_0063fb88 = 0. Its only caller in the exe is the matched 0x004210b0 (Match/PortTop_004210b0.cpp, MASHED_ORIG_TOP path,
// from the matched 0x0040bbb0). No callees. The other body of this RVA is the .asi-only Match/MatchS05_00421080.cpp
// (Fill63e5a4), so this TU is the ORIGTOP-ONLY class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00421080 Orig_00421080
extern "C" {
#include "../../../../re/match/s05/00421080.c"
}
#endif  // MASHED_STANDALONE
