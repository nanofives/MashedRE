// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x004540c0 = the matched source re/match/s09/004540c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: RpClumpDestroy over 0x00688260 step 0x2c B; over 0x00688058 step 0x44 B: RwFrameRemoveChild, RwFrameDestroy x2, RpAtomicDestroy; RpClumpDestroy(DAT_006882f4), RpClumpDestroy(DAT_006882f0).
// Teardown target of powerup type table entry 1: the dword at 0x005f99f0 (0x005f99b0 + 0x40*1) holds 0x004540c0 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_004540c0: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
//   0x004c0c20 = MashedRw_FrameDestroy_004c0c20 (RwFrameDestroy, LibRw/RwApiBind.cpp).
//   0x004c1210 = MashedRw_FrameRemoveChild_004c1210 (RwFrameRemoveChild, LibRw/RwApiBind.cpp).
//   0x004e6920 = MashedRw_AtomicDestroy_004e6920 (RpAtomicDestroy, LibRw/RwApiBind.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004540c0 Orig_004540c0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_004c0c20 MashedRw_FrameDestroy_004c0c20
#define FUN_004c1210 MashedRw_FrameRemoveChild_004c1210
#define FUN_004e6920 MashedRw_AtomicDestroy_004e6920
extern "C" {
#include "../../../../re/match/s09/004540c0.c"
}
#endif  // MASHED_STANDALONE
