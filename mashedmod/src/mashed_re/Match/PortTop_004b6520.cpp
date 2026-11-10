// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x004b6520 = the matched source re/match/s18/004b6520.c
// (FUN_004b64e0(dst, 0, size)). Called by the matched 0x00420d00 (Match/PortTop_00420d00.cpp) on the MASHED_ORIG_TOP
// path. The callee 0x004b64e0 is the exe body MemsetInline (Input/MemsetInline_ag1.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b6520 Orig_004b6520
#define FUN_004b64e0 MemsetInline                           // exe body, Input/MemsetInline_ag1.cpp
extern "C" {
#include "../../../../re/match/s18/004b6520.c"
}
#endif  // MASHED_STANDALONE
