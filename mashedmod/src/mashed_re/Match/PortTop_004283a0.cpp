// PORT-TOP-04F (fonts family): exe-only port body of rva 0x004283a0 = the matched source re/match/s06/004283a0.c
// (match.py BYTE-IDENTICAL 92 B, relocheck OK 15/15). Called by the matched 0x00402750 (Match/PortTop_00402750.cpp) on
// the MASHED_ORIG_TOP path, with font36.piz mounted. It reads four textures (MashedNEWLogo, bigE at 0x005cd640,
// proLogicII, DolbyDig) into DAT_0067d964 / 70 / 68 / 6c and copies the first into DAT_00771964. Callees are librw
// bindings (LibRw/RwApiBind.cpp): RwImageSetPath and RwTextureRead.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004283a0 Orig_004283a0
#define FUN_004ce2d0 MashedRw_ImageSetPath_004ce2d0
#define FUN_004c5cb0 MashedRw_TextureRead_004c5cb0
extern "C" {
#include "../../../../re/match/s06/004283a0.c"
}
#endif  // MASHED_STANDALONE
