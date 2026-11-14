// PORT-TOP-AUDIOTICK-IMGTRAP-4568A0 (L-111 top-down lane): exe-only port body of rva 0x004568a0 = the matched source
// re/match/s09/004568a0.c (C4 by route match, BYTE-IDENTICAL 43 B, relocheck OK checked=4). Loop from 0x006886b8 in
// 0x48-byte steps while below 0x006887d8: RpClumpDestroy(*p) (4 calls), then FUN_004768c0(0x006887d0).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045b930 (Match/PortTop_0045b930.cpp), through the
// teardown table 0x005f99b0 entry 0 (stride 0x40, count DAT_005f9bd8 = 9). The slot 0x005f99b0 is bound to this
// body by the code-pointer fix-up (Compat/PortTopCodePtr.inc, verify/portorigtop_20261109/codeptr_inventory.py),
// which before this row bound it to the stop thunk PortTopCodePtr_Stop_004568a0.
// Callees bind by name, no raw-address call:
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp, existing binding, no new librw binding)
//  - 0x004768c0 = Orig_004768c0 (Match/PortTop_004768c0.cpp, matched)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004568a0 Orig_004568a0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_004768c0 Orig_004768c0
extern "C" {
#include "../../../../re/match/s09/004568a0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
