// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x0045a790 = the matched source re/match/s09/0045a790.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: from 0x0068bb70 step 0x68 B while < 0x0068bd10: RpClumpDestroy(p[4]), 0x004898d0(*p).
// Teardown target of powerup type table entry 4: the dword at 0x005f9ab0 (0x005f99b0 + 0x40*4) holds 0x0045a790 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_0045a790: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
//   0x004898d0 = Orig_004898d0 (Match/PortTop_004898d0.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045a790 Orig_0045a790
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_004898d0 PortTop_0045a790_Call004898d0
extern "C" {
void Orig_004898d0(void* p0);   // matched, Match/PortTop_004898d0.cpp
void PortTop_0045a790_Call004898d0(int p0) { Orig_004898d0(reinterpret_cast<void*>(static_cast<unsigned long>(p0))); }
#include "../../../../re/match/s09/0045a790.c"
}
#endif  // MASHED_STANDALONE
