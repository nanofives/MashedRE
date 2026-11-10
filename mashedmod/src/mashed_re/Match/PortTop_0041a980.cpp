// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x0041a980 = the matched source re/match/s03/raw_0041a980.c
// (BYTE-IDENTICAL 45 B, relocheck OK): RpClumpDestroy on the two dwords at 0x0063c7b8, then 0x004b6520(0x0063c7b8, 8).
// The matched 0x0040cf80 (Match/PortTop_0040cf80.cpp) calls it on the MASHED_ORIG_TOP path (AppDestroy 0x00402a40), only
// when 0x0041f320(i) is nonzero for some i < 4.
// The source's raw g2s preamble became the MATCH-S02 preamble and its `extern` global an MGLOBAL (the 0x0041a3d0 form),
// C text under the DDK unchanged (match.py --bytes BYTE-IDENTICAL before and after).
// Callees bind by name, no raw-address call:
//  - RpClumpDestroy (0x004e6e00) -> MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp, rw::Clump::destroy, null skipped).
//  - 0x004b6520 -> Orig_004b6520 (Match/PortTop_004b6520.cpp), through a C++ overload taking the pointer (the source declares
//    undefined4 parameters and passes &DAT_0063c7b8).
// Shared state: no exe TU reads or writes 0x0063c7b8 / 0x0063c7bc, so both stay 0 on this path.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004b6520(unsigned int dst, unsigned int size);
static inline void PortTop_0041a980_Call004b6520(void* p0, unsigned int p1) {
    Orig_004b6520(static_cast<unsigned int>(reinterpret_cast<unsigned long>(p0)), p1);
}
#define FUN_0041a980 Orig_0041a980
#define FUN_004b6520 PortTop_0041a980_Call004b6520
#define RpClumpDestroy MashedRw_ClumpDestroy_004e6e00           // RW: RpClumpDestroy 0x004e6e00 -> rw::Clump::destroy
extern "C" {
#include "../../../../re/match/s03/raw_0041a980.c"
}
#endif  // MASHED_STANDALONE
