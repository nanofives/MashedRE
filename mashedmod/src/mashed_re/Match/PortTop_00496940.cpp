// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00496940 = the matched source re/match/s16/00496940.c
// (DAT_00773078 = DAT_00771a0c; DAT_0077307c = RwRasterCreate(0, 0, 0, 0x84); returns 1). Called only by the matched
// 0x0047ba00 (Match/PortTop_0047ba00.cpp) on the MASHED_ORIG_TOP path. The matched teardown 0x00496970
// (Match/PortTop_00496970.cpp) destroys DAT_0077307c when nonzero.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00496940 Orig_00496940
#define FUN_004c77c0 MashedRw_RasterCreate_004c77c0          // RW: RwRasterCreate -> rw::Raster::create
extern "C" {
#include "../../../../re/match/s16/00496940.c"
}
#endif  // MASHED_STANDALONE
