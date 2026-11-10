// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x004b6480 BitArrayClear = the matched source
// re/match/s18/004b6480.c (MATCH-S18 h4, 100%): clears the first param_2 bits of param_1 (memset of param_2 >> 3 bytes,
// then a mask over the remaining bits of the next byte). Its only caller in the exe is the matched 0x004210b0
// (Match/PortTop_004210b0.cpp, MASHED_ORIG_TOP path), on the bit array 0x0063e4a8 with 0x80 bits. No callees (memset is
// the compiler intrinsic). The other body of this RVA is the .asi-only Input/DirectInput.cpp BitArrayClear, so this TU is
// the ORIGTOP-ONLY class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b6480 Orig_004b6480
extern "C" {
#include "../../../../re/match/s18/004b6480.c"
}
#endif  // MASHED_STANDALONE
