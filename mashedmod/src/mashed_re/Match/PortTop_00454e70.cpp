// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x00454e70 = the matched source re/match/s29/00454e70.c
// (POWERUPINIT-B, BYTE-IDENTICAL). flashunit.dff clones in the four 0x14-byte entries at 0x00688358, atoms
// filed by part code, the 0x0088fec0 lensflare batch with 0x00476c10(batch, {0, 0, 0.5, 1}), then the tail jump to
// 0x00454e40.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 6 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp)
//  - 0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004b5240 = Orig_004b5240 (Match/PortTop_004b5240.cpp)
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp)
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp)
//  - 0x00476c10 = Batch476c10 (Render/RfxEmitter.cpp (writes the PTank block of the 0x004770c0 record))
//  - 0x00454e40 = Orig_00454e40 (Match/PortTop_00454e40.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00454e70 Orig_00454e70
#define FUN_004b6520 Orig_004b6520
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004b5240 Orig_004b5240
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_00476c10 Batch476c10
#define FUN_00454e40 Orig_00454e40
extern "C" {
#include "../../../../re/match/s29/00454e70.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
