// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x00477b40 = the matched source re/match/s12/00477b40.c
// (MATCH-S12, BYTE-IDENTICAL): 0x004b6520(0x00693198, 0x2c000) and 0x004b6520(0x006bf198, 0x20). Its only caller on the
// MASHED_ORIG_TOP path is the matched 0x00477e40 (Match/PortTop_00477e40.cpp), called by the matched 0x0040bbb0.
// Callee binds by name, no raw-address call: 0x004b6520 -> Orig_004b6520 (Match/PortTop_004b6520.cpp).
// Shared state: no other exe TU reads or writes 0x00693198..0x006bf197 or 0x006bf198..0x006bf1b7.
// The other body of this RVA is the .asi-only Match/MatchS12_00477b40.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00477b40 Orig_00477b40
#define FUN_004b6520 Orig_004b6520                              // matched, Match/PortTop_004b6520.cpp
extern "C" {
#include "../../../../re/match/s12/00477b40.c"
}
#endif  // MASHED_STANDALONE
