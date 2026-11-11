// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00552a60 = the matched source re/match/s24/00552a60.c (100%, 236 B BYTE-IDENTICAL, relocheck OK).
// Rt2d camera: for a non-zero camera with a raster, the size floats, the callback swap at camera +0x1c to 0x00552b50 (the old
// one kept at 0x00912c58), DAT_00912c0c = camera, aspect at 0x00912c00 / 0x00912c04, returns 1; a camera with no raster
// returns 0; camera 0 sets DAT_00912c0c = 0 and returns 0.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
// On this path the camera is 0 (0x00427ca0 passes 0 to 0x00552b60, whose tail jmp at 0x00552b88 hands it on).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00552a60 Orig_00552a60
#define FUN_00552b50 Orig_00552b50   // matched, Match/PortTop_00552b50.cpp
extern "C" {
#include "../../../../re/match/s24/00552a60.c"
}
#endif  // MASHED_STANDALONE
