// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x0048fed0 = the matched source re/match/s15/0048fed0.c
// (BYTE-IDENTICAL 12 B, relocheck OK): the RpClumpForAllAtomics callback of 0x0048ff20 (push at 0x0048ff89), stores its
// atomic at data +0x08 (0x0048fed8) and returns it. On the MASHED_ORIG_TOP path the atomic is the librw rw::Atomic the
// RpClumpForAllAtomics binding hands over (LibRw/RwApiBind.cpp MashedRw_ClumpForAllAtomics_004e66d0), which a callback
// may store but not read: this one only stores it.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048fed0 Orig_0048fed0
extern "C" {
#include "../../../../re/match/s15/0048fed0.c"
}
#endif  // MASHED_STANDALONE
