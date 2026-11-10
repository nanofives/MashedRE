// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x0045b810 = the matched source re/match/s29/0045b810.c
// (POWERUPINIT-B, BYTE-IDENTICAL). Tail-jump target of 0x0045b830: the static 0x0045b360 (ESI) over the four
// 0x24-byte slots at 0x0068d158.
// Callees bind by name, no raw-address call:
//  - 0x004e4800 = MashedRw_ClumpGetWorld_004e4800 (LibRw/RwApiBind.cpp)
//  - 0x004e45b0 = MashedRw_WorldRemoveClump_004e45b0 (LibRw/RwApiBind.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045b810 Orig_0045b810
#define FUN_004e4800 MashedRw_ClumpGetWorld_004e4800
#define FUN_004e45b0 MashedRw_WorldRemoveClump_004e45b0
extern "C" {
#include "../../../../re/match/s29/0045b810.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
