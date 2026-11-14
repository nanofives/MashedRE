// IMG-5-TEARDOWN-BODIES (L-151 (a), run 45): exe-only port body of rva 0x00456ca0 = the matched source re/match/s09/00456ca0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py 100%, relocheck OK; verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-BODIES.md).
// Body: RpClumpDestroy over 0x0068a25c step 0x10 B while < 0x0068a29c, 0x004768c0(&0x0068a2c0), 0x004768c0(&0x0068a210).
// Teardown target of powerup type table entry 7: the dword at 0x005f9b70 (0x005f99b0 + 0x40*7) holds 0x00456ca0 in the original
// (MASHED.exe.unpatched). Its caller is the matched teardown 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40)
// through that slot, so the exe reaches this body only when the slot holds Orig_00456ca0: Compat/PortTopCodePtr.inc
// (slot 0x005f99b0) or Compat/PortTopCodePtrImage.inc (slots 1..8), MASHED_ORIG_TOP with MASHED_OT_CODEPTR=1.
// Callees bind by name, no raw-address call:
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (RpClumpDestroy, LibRw/RwApiBind.cpp (rw::Clump::destroy, null skipped)).
//   0x004768c0 = Orig_004768c0 (Match/PortTop_004768c0.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00456ca0 Orig_00456ca0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
#define FUN_004768c0 PortTop_00456ca0_Call004768c0
extern "C" {
void Orig_004768c0(int p0);   // matched, Match/PortTop_004768c0.cpp
void PortTop_00456ca0_Call004768c0(void* p0) { Orig_004768c0(static_cast<int>(reinterpret_cast<unsigned long>(p0))); }
#include "../../../../re/match/s09/00456ca0.c"
}
#endif  // MASHED_STANDALONE
