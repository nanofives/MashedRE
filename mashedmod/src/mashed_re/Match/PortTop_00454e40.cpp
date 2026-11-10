// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x00454e40 = the matched source re/match/s29/00454e40.c
// (POWERUPINIT-B, BYTE-IDENTICAL). Tail-jump target of 0x00454e70: the static 0x00454bd0 (ESI) over the
// four entries at 0x00688358 and 0x00454c00 (EAX) over the four 0x18-byte records at 0x006882f8.
// Callees bind by name, no raw-address call:
//  - 0x004e4800 = MashedRw_ClumpGetWorld_004e4800 (LibRw/RwApiBind.cpp)
//  - 0x004e45b0 = MashedRw_WorldRemoveClump_004e45b0 (LibRw/RwApiBind.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00454e40 Orig_00454e40
#define FUN_004e4800 MashedRw_ClumpGetWorld_004e4800
#define FUN_004e45b0 MashedRw_WorldRemoveClump_004e45b0
extern "C" {
#include "../../../../re/match/s29/00454e40.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
