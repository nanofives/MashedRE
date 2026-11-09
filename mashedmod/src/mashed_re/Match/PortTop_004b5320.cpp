// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004b5320 (RpClumpForAllAtomics(clump, 0x004b5300, {mask, set})) = the matched
// source re/match/s18/004b5320.c (match.py BYTE-IDENTICAL 43 B). Callees are bindings (LibRw/RwApiBind.cpp):
// RpClumpForAllAtomics 0x004e66d0 (librw atomics in RW order) and the callback 0x004b5300 (no matched source).
// Caller on the MASHED_ORIG_TOP path: 0x00401000 (cup.dff, mask 0x40, set 1).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b5320 Orig_004b5320
#define FUN_004b5300 MashedRw_AtomicGeometryFlags_004b5300
#define FUN_004e66d0 MashedRw_ClumpForAllAtomics_004e66d0
extern "C" {
#include "../../../../re/match/s18/004b5320.c"
}
#endif  // MASHED_STANDALONE
