// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004583a0 = the matched source re/match/s29/004583a0.c
// (POWERUPINIT-B, BYTE-IDENTICAL). MineDropper.dff clones in the four 0x18-byte dropper records at 0x0068b0d0,
// Mine.dff clones in the 32 0x6c-byte mine records at 0x0068a310 with atoms filed by part code, 0x00458360(), and the
// 0x0068b090 lensflare batch with 0x00476c10(batch, 0x005f9820).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 3 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp)
//  - 0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp)
//  - 0x004b5190 = MashedRw_AtomicPartCode_004b5190 (LibRw/RwApiBind.cpp)
//  - 0x004b5240 = Orig_004b5240 (Match/PortTop_004b5240.cpp)
//  - 0x00458360 = Orig_00458360 (Match/PortTop_00458360.cpp)
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp)
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp)
//  - 0x00476c10 = Batch476c10 (Render/RfxEmitter.cpp (writes the PTank block of the 0x004770c0 record))
// 0x005f9820 is .data of the original: seeded (Compat/PortStateSeed.inc, this row).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004583a0 Orig_004583a0
#define FUN_004b6520 Orig_004b6520
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004b5240 Orig_004b5240
#define FUN_00458360 Orig_00458360
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_00476c10 Batch476c10
extern "C" {
#include "../../../../re/match/s29/004583a0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
