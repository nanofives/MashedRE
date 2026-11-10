// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004b3f20 = the matched source re/match/s29/004b3f20.c
// (POWERUPINIT-C, BYTE-IDENTICAL 34 B, relocheck OK): the RpClumpForAllAtomics callback of 0x004b3fc0. Context
// {out, &count}: when out is non-zero, out[*count] = atomic; then ++*count; returns the atomic. Caller on the
// MASHED_ORIG_TOP path: the MashedRw_ClumpForAllAtomics_004e66d0 binding, on behalf of the matched 0x004b3fc0
// (Match/PortTop_004b3fc0.cpp), which passes its address. No callee.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b3f20 Orig_004b3f20
extern "C" {
#include "../../../../re/match/s29/004b3f20.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
