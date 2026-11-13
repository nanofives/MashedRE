// PORT-TOP-04K (L-96 top-down lane): exe-only port body of rva 0x004b51d0 = the matched source re/match/s18/004b51d0.c
// (match.py --bytes: whole obj symbol BYTE-IDENTICAL 98 B). Collects the clump's atomics through 0x004b3fc0 into a
// 256-entry list, then stores each atomic at out[0x004b5190(atomic, a4, a5)]. Caller on the MASHED_ORIG_TOP path: the
// matched startlights loader 0x0041d8b0 (Match/PortTop_0041d8b0.cpp), from 0x00402750.
// Callees bind by name (extern "C", cdecl, every argument a 4-byte word), no raw-address call:
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp, librw atomics through the RpClumpForAllAtomics binding)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp, user-data element as int)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b51d0 Orig_004b51d0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
extern "C" {
#include "../../../../re/match/s18/004b51d0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
