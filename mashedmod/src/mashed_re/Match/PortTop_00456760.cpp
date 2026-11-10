// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x00456760 = the matched source re/match/s09/00456760.c
// (POWERUPINIT-B, BYTE-IDENTICAL). GatlingGun.dff: "flash" particle batch 0x006887d0, four clones in 0x48-byte
// records from 0x00688670 with their atoms filed by part code 0..6, 0x00474d60(clump, 0) per clone, the source clump
// destroyed. The source is the MATCH-S09 file (100%).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 0 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp)
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp)
//  - 0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x00474d60 = Orig_00474d60 (Match/PortTop_00474d60.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00456760 Orig_00456760
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_00474d60 Orig_00474d60
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s09/00456760.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
