// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x004530b0 = the matched source re/match/s09/004530b0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: RpClumpDestroy over 0x00684e4c step 0x1c B (< 0x00684ebc); from 0x00684f00 step 0x110 B (< 0x00687100): RpClumpDestroy(p[-4]), 0x004898d0(*p).
// Teardown target of powerup type table entry 8: the dword at 0x005f9bb0 (0x005f99b0 + 0x40*8) holds 0x004530b0 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_004530b0: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
//   0x004898d0 = Orig_004898d0 (Match/PortTop_004898d0.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004530b0 Orig_004530b0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_004898d0 PortTop_004530b0_Call004898d0
extern "C" {
void Orig_004898d0(void* p0);   // matched, Match/PortTop_004898d0.cpp
void PortTop_004530b0_Call004898d0(int p0) { Orig_004898d0(reinterpret_cast<void*>(static_cast<unsigned long>(p0))); }
#include "../../../../re/match/s09/004530b0.c"
}
#endif  // MASHED_STANDALONE
