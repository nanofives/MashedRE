// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004014b0 (the cup world teardown) = the matched source
// re/match/s01/raw/004014b0.c (match.py BYTE-IDENTICAL 182 B): RpWorldRemoveLight x2, RwFrameDestroy of each light's frame
// (+0x04, when nonzero), 0x004c0740(light, 0), RpLightDestroy, RpWorldRemoveClump, RpWorldDestroy, RpClumpDestroy.
// Reached only on the MASHED_ORIG_TOP path, from the matched AppDestroy 0x00402a40 (Match/PortTop_00402a40.cpp), after
// 0x00401000 built that state. Callees are RW bindings (LibRw/RwApiBind.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004014b0 Orig_004014b0
#define RpWorldRemoveLight MashedRw_WorldRemoveLight_004e4860   // RW: RpWorldRemoveLight 0x004e4860
#define FUN_004c0c20 MashedRw_FrameDestroy_004c0c20             // RW: RwFrameDestroy
#define FUN_004c0740 MashedRw_ObjectHasFrameSetFrame_004c0740   // RW: _rwObjectHasFrameSetFrame (light record case)
#define FUN_004e4d90 MashedRw_LightDestroy_004e4d90             // RW: RpLightDestroy
#define RwFrameRemoveChild MashedRw_WorldRemoveClump_004e45b0   // rva 0x004e45b0 (hooks.csv name RwFrameRemoveChild; world, clump)
#define RpWorldDestroy MashedRw_WorldDestroy_004e5700           // RW: RpWorldDestroy (PORT-TOP-04C)
#define RpClumpDestroy MashedRw_ClumpDestroy_004e6e00           // RW: RpClumpDestroy (PORT-TOP-04C)
extern "C" {
#include "../../../../re/match/s01/raw/004014b0.c"
}
#endif  // MASHED_STANDALONE
