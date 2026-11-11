// HUDINIT-B (L-111 step 1, chain 2 of 0x00427ca0 FontText_HudInit: brushes + the 0x00427ca0 body): exe-only port body of
// rva 0x00556d70 = the matched source re/match/rwl05/00556d70.c (100%, 195 B BYTE-IDENTICAL, relocheck OK 5/5).
// Brush create: one 0x78-byte entry from the brush list DAT_00912a00 through RwGlobals +0x118 (hint 0x301a2), zeroed
// (0x1e dwords), colours (255, 255, 255, 255) x4 through 0x00556e90, UVs (0, 0) x4 through 0x00557110, +0x64 bit 1
// cleared, +0x68 texture destroyed when set (0 after the zeroing) and cleared, +0x70 = 0.5f, +0x74 = 1. Returns the
// entry, or 0 when the alloc fails.
// Reached only from the matched 0x00427ca0 (Match/MatchS06_00427ca0.cpp), which the exe runs only on the MASHED_ORIG_TOP
// path with MASHED_ORIG_TOP_RWMOD=1 (selector PortTop_00402750_Call00427ca0, Match/PortTop_00552b60.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00556d70 Orig_00556d70
// FUN_004c5a60: RW: RwTextureDestroy -> rw::Texture::destroy (LibRw/RwApiBind.cpp)
#define FUN_004c5a60 MashedRw_TextureDestroy_004c5a60
// FUN_00556e90: stand-in, Match/PortTop_StandInsHIB.cpp: re/match/s24/00556e90.c is 97.5%, residual outside L-115 (RESULT_HUDINIT-B.md)
#define FUN_00556e90 PortTop_StandInHIB_00556e90
// FUN_00557110: matched, Match/PortTop_00557110.cpp
#define FUN_00557110 Orig_00557110
extern "C" {
#include "../../../../re/match/rwl05/00556d70.c"
}
#endif  // MASHED_STANDALONE
