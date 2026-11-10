// ATOMIC-VIEW-2 (L-111 step 1): exe-only port body of rva 0x004548e0 = the matched source re/match/s09/004548e0.c
// (MATCH-S09, C4): the DepthCharge type-table init (entry 1). Zero-fills the four 0x2c-byte entries at 0x00688240 and the
// eight 0x44-byte round records at 0x00688020; four clones of "DepthCharge.dff" at entry +0x20 with their atoms filed by
// part code at +0x14; then "DepthChargeRound.dff", its first atom (0x004b3f90, the RW-layout view) cloned eight times
// into round +0x30, each with a new frame (+0x38, the atom's frame) under a second new frame (+0x34); then 0x004548a0()
// and RwV3dNormalize(0x005f9808, 0x005f9808). Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through
// type-table entry 1 (+0x14), stored by PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x004e69a0 RpAtomicClone, 0x004e7e30 RpAtomicSetFrame (librw atomics), 0x004c1040 RwFrameAddChild, 0x004c39b0
//    RwV3dNormalize: the ATOMIC-VIEW-2 bindings in LibRw/RwApiBind.cpp. 0x004e69a0 resolves the view to its librw atomic.
//  - 0x004b3f90 = MashedRw_ClumpFirstAtomic_004b3f90 (the view), 0x004c0b30 = MashedRw_FrameCreate_004c0b30, 0x004e6ab0 =
//    MashedRw_ClumpClone_004e6ab0, 0x004b5190 = MashedRw_AtomicPartCode_004b5190, 0x004c5c80 =
//    MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp).
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp), 0x004b3fc0 = Orig_004b3fc0, 0x004b5240 = Orig_004b5240,
//    0x004b6520 = Orig_004b6520, 0x004548a0 = Orig_004548a0 (Match/PortTop_004548a0.cpp).
// 0x005f9808 (12 B .data, {8, 1, 0}) is seeded (Compat/PortStateSeed.inc, ATOMIC-VIEW-2).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004548e0 Orig_004548e0
#define FUN_004548a0 Orig_004548a0
#define FUN_004b3f90 MashedRw_ClumpFirstAtomic_004b3f90
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_004b5240 Orig_004b5240
#define FUN_004b6520 Orig_004b6520
#define FUN_004c0b30 MashedRw_FrameCreate_004c0b30
#define FUN_004c1040 MashedRw_FrameAddChild_004c1040
#define FUN_004c39b0 MashedRw_V3dNormalize_004c39b0
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e69a0 MashedRw_AtomicClone_004e69a0
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004e7e30 MashedRw_AtomicSetFrame_004e7e30
extern "C" {
#include "../../../../re/match/s09/004548e0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
