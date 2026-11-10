// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x00452f70 = the matched source re/match/s29/00452f70.c
// (POWERUPINIT-B, BYTE-IDENTICAL). four mortar.dff clones in the 0x1c-byte launcher records from 0x00684e3c
// (atom of part code 2 filed), 32 MortarRound.dff clones in the 0x110-byte round records from 0x00684eac, each with
// a "smoke" particle pool.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 8 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x00474d60 = Orig_00474d60 (Match/PortTop_00474d60.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004b5240 = Orig_004b5240 (Match/PortTop_004b5240.cpp)
//  - 0x004892c0 = Orig_004892c0 (Match/PortTop_004892c0.cpp)
//  - 0x004893a0 = Orig_004893a0 (Match/PortTop_004893a0.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00452f70 Orig_00452f70
#define FUN_0040bb30 Search40bb30
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_00474d60 Orig_00474d60
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004b5240 Orig_004b5240
#define FUN_004892c0 Orig_004892c0
#define FUN_004893a0 Orig_004893a0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/00452f70.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
