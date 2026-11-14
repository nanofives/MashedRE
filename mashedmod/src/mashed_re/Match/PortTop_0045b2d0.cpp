// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x0045b2d0 = the matched source re/match/s09/0045b2d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: RpClumpDestroy over 0x0068d174 step 0x24 B while < 0x0068d204, then RpClumpDestroy(DAT_0068d1e8).
// Teardown target of powerup type table entry 5: the dword at 0x005f9af0 (0x005f99b0 + 0x40*5) holds 0x0045b2d0 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_0045b2d0: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045b2d0 Orig_0045b2d0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s09/0045b2d0.c"
}
#endif  // MASHED_STANDALONE
