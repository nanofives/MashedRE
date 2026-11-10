// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004b3fc0 = the matched source re/match/s29/004b3fc0.c
// (POWERUPINIT-B, BYTE-IDENTICAL 55 B): count = 0, context {out, &count}, RpClumpForAllAtomics(clump, 0x004b3f20,
// &context), returns count. Callers on the MASHED_ORIG_TOP path: the matched type-table inits 0x00456760, 0x004583a0,
// 0x0045a6b0, 0x0045b830, 0x00454e70, 0x00452f70 (Match/PortTop_<rva>.cpp), from 0x0045bae0.
// Callees bind by name, no raw-address call:
//  - 0x004e66d0 RpClumpForAllAtomics = MashedRw_ClumpForAllAtomics_004e66d0 (LibRw/RwApiBind.cpp: librw atomics in RW
//    order), through the call-through below.
//  - 0x004b3f20 (the callback) = Orig_004b3f20 (Match/PortTop_004b3f20.cpp = re/match/s29/004b3f20.c, BYTE-IDENTICAL).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b3f20 Orig_004b3f20
#define FUN_004e66d0 PortTop_004b3fc0_Call004e66d0
extern "C" {
#include "../../../../re/match/s29/004b3fc0.c"
int MashedRw_ClumpForAllAtomics_004e66d0(int clump, int cb, int data);
// RpClumpForAllAtomics call-through (not a body): the source's typed arguments as the binding's ints.
void *PortTop_004b3fc0_Call004e66d0(void *clump, void *(*cb)(void *, Ctx004b3fc0 *), void *data) {
    return reinterpret_cast<void *>(static_cast<unsigned long>(static_cast<unsigned>(MashedRw_ClumpForAllAtomics_004e66d0(
        static_cast<int>(reinterpret_cast<unsigned long>(clump)), static_cast<int>(reinterpret_cast<unsigned long>(cb)),
        static_cast<int>(reinterpret_cast<unsigned long>(data))))));
}
}  // extern "C"
#endif  // MASHED_STANDALONE
