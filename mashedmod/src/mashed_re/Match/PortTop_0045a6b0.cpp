// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x0045a6b0 = the matched source re/match/s29/0045a6b0.c
// (POWERUPINIT-B, BYTE-IDENTICAL). FlameThrower.dff clones in four 0x68-byte records from 0x0068bb60, the
// part-code-1 atom's frame LTM copied in, 0x0045a3a0(record), an "exp_cloud2" particle pool per record.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 4 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004c0ed0 = MashedRw_FrameGetLTM_004c0ed0 (LibRw/RwApiBind.cpp)
//  - 0x0045a3a0 = PortTop_0045a3a0_Copy (Match/PortTop_0045a3a0.cpp)
//  - 0x004892c0 = Orig_004892c0 (Match/PortTop_004892c0.cpp)
//  - 0x004893a0 = Orig_004893a0 (Match/PortTop_004893a0.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0045a6b0 Orig_0045a6b0
#define FUN_0040bb30 Search40bb30
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004c0ed0 MashedRw_FrameGetLTM_004c0ed0
#define FUN_0045a3a0 PortTop_0045a3a0_Copy
#define FUN_004892c0 Orig_004892c0
#define FUN_004893a0 Orig_004893a0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/0045a6b0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
