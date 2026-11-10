// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x00474d60 = the matched source re/match/s11/00474d60.c
// (MATCH-S11, 100%): when the clump is non-zero, RpClumpForAllAtomics(clump, 0x00474d30, &flag). The callback
// 0x00474d30 (same source, no Ghidra function) sets bit 4 of the atomic's flags byte (+0x02) when flag != 0, else
// clears it, and returns the atomic. Caller on the MASHED_ORIG_TOP path: the matched builder 0x00459290 (four lazer.dff
// clumps, flag 0, Match/PortTop_00459290.cpp) from 0x0045bae0.
// Callees bind by name: RpClumpForAllAtomics 0x004e66d0 = MashedRw_ClumpForAllAtomics_004e66d0 (LibRw/RwApiBind.cpp:
// librw atomics in RW order). The callback runs on the librw rw::Atomic. Its +0x02 byte is rw::Object::flags
// (deps/librw/src/rwobjects.h, Object {type, subType, flags, privateFlags, parent}), and bit 4 there is
// rw::Atomic::RENDER, the same bit as RW's rpATOMICRENDER.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00474d60 Orig_00474d60
#define FUN_00474d30 Orig_00474d30
#define FUN_004e66d0 PortTop_00474d60_Call004e66d0
extern "C" {
#include "../../../../re/match/s11/00474d60.c"
int MashedRw_ClumpForAllAtomics_004e66d0(int clump, int cb, int data);
// RpClumpForAllAtomics call-through (not a body): the source's typed callback and data pointer as the binding's ints.
int PortTop_00474d60_Call004e66d0(int clump, AtomicCb cb, int* data) {
    return MashedRw_ClumpForAllAtomics_004e66d0(clump, static_cast<int>(reinterpret_cast<unsigned long>(cb)),
                                                static_cast<int>(reinterpret_cast<unsigned long>(data)));
}
}  // extern "C"
#endif  // MASHED_STANDALONE
