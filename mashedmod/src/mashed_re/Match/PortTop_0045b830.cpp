// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x0045b830 = the matched source re/match/s29/0045b830.c
// (POWERUPINIT-B, BYTE-IDENTICAL). shotgun.dff clones in the four 0x24-byte slots at 0x0068d158, atoms filed by
// part code, codes 1..2 scaled {4, 4, 4} and given geometry flag 0x40, then the tail jump to 0x0045b810.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 5 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp)
//  - 0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004b5240 = Orig_004b5240 (Match/PortTop_004b5240.cpp)
//  - 0x004c13e0 = MashedRw_FrameScale_004c13e0 (LibRw/RwApiBind.cpp)
//  - 0x004b52f0 = MashedRw_AtomicGeometryFlags_004b52f0 (LibRw/RwApiBind.cpp)
//  - 0x0045b810 = Orig_0045b810 (Match/PortTop_0045b810.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045b830 Orig_0045b830
#define FUN_004b6520 Orig_004b6520
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004b5240 Orig_004b5240
#define FUN_004c13e0 MashedRw_FrameScale_004c13e0
#define FUN_004b52f0 MashedRw_AtomicGeometryFlags_004b52f0
#define FUN_0045b810 Orig_0045b810
extern "C" {
#include "../../../../re/match/s29/0045b830.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
