// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x0048fee0 = the matched source re/match/s15/0048fee0.c
// (BYTE-IDENTICAL 12 B, relocheck OK): the RpGeometryForAllMaterials callback of 0x0048fef0 (push 0x48fee0 at
// 0x0048ff01), stores its material at data +0x0c (0x0048fee8) and returns it. The only writer of the slot words
// 0x0086a4ac.. (RESULT_PORT-OT-RENDER-2.md item 2). On the MASHED_ORIG_TOP path the material is the librw rw::Material
// the 0x004e89a0 binding hands over, whose texture sits at +0x00 as in RW 3.x (static_assert in LibRw/RwApiBind.cpp), so
// the readers 0x00490000 / 0x0048ffd0 can pass it to the matched RpMaterialSetTexture 0x004e8090.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048fee0 Orig_0048fee0
extern "C" {
#include "../../../../re/match/s15/0048fee0.c"
}
#endif  // MASHED_STANDALONE
