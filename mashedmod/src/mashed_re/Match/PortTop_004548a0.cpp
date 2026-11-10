// ATOMIC-VIEW-2 (L-111 step 1): exe-only port body of rva 0x004548a0 = the matched source re/match/s09/004548a0.c
// (MATCH-S09, C4), with its statics 0x004547c0 (entry reset: the two round pointers through 0x00454170, fields zeroed,
// the clone at +0x20 detached from its world) and 0x00454170 (round reset: both frames set to identity, the +0x34 frame
// removed from its parent when it has one, the +0x30 atomic removed from its world). Caller on the MASHED_ORIG_TOP path:
// the matched 0x004548e0 (Match/PortTop_004548e0.cpp), right after it filled both arrays.
// Callees bind by name: 0x004c15c0 RwFrameSetIdentity, 0x004c1210 RwFrameRemoveChild, 0x004e4440 RpAtomicGetWorld,
// 0x004e43b0 RpWorldRemoveAtomic (ATOMIC-VIEW-2 bindings), 0x004e4800 = MashedRw_ClumpGetWorld_004e4800, 0x004e45b0 =
// MashedRw_WorldRemoveClump_004e45b0 (LibRw/RwApiBind.cpp), 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004548a0 Orig_004548a0
#define FUN_004c15c0 MashedRw_FrameSetIdentity_004c15c0
#define FUN_004c1210 MashedRw_FrameRemoveChild_004c1210
#define FUN_004e4440 MashedRw_AtomicGetWorld_004e4440
#define FUN_004e43b0 MashedRw_WorldRemoveAtomic_004e43b0
#define FUN_004e4800 MashedRw_ClumpGetWorld_004e4800
#define FUN_004e45b0 MashedRw_WorldRemoveClump_004e45b0
#define FUN_004b6520 Orig_004b6520
extern "C" {
#include "../../../../re/match/s09/004548a0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
