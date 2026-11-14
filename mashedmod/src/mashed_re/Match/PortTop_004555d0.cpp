// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x004555d0 = the matched source re/match/s09/004555d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: from 0x006885e8 step 0x2c B and 0x00688418 step 0x6c B while < 0x00688634: RpClumpDestroy(*p), RpClumpDestroy(q[-3]), 0x00489c60(*q).
// Teardown target of powerup type table entry 2: the dword at 0x005f9a30 (0x005f99b0 + 0x40*2) holds 0x004555d0 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_004555d0: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
//   0x00489c60 = Orig_00489c60 (Match/PortTop_00489c60.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004555d0 Orig_004555d0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_00489c60 PortTop_004555d0_Call00489c60
extern "C" {
void Orig_00489c60(void* p0);   // matched, Match/PortTop_00489c60.cpp
void PortTop_004555d0_Call00489c60(int p0) { Orig_00489c60(reinterpret_cast<void*>(static_cast<unsigned long>(p0))); }
#include "../../../../re/match/s09/004555d0.c"
}
#endif  // MASHED_STANDALONE
