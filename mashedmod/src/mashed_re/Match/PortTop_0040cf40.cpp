// PORT-OT-GAMEPLAY (L-111 (a) step 1): exe-only port body of rva 0x0040cf40 = the matched source re/match/s02/0040cf40.c
// (MATCH-S02, match.py 100%): RwTextureDestroy (0x004c5a60) on DAT_0063ba90 and DAT_0063ba94 when nonzero, each cleared
// after. Called on the MASHED_ORIG_TOP path by the matched 0x0040cf80 (Match/PortTop_0040cf80.cpp), when 0x0041f320(i) is
// nonzero for some i < 4. Callee binds by name: 0x004c5a60 -> MashedRw_TextureDestroy_004c5a60 (LibRw/RwApiBind.cpp,
// rw::Texture::destroy) through a forwarder with the source prototype. The other body of 0x0040cf40 is the .asi-only
// Match/MatchS02_0040cf40.cpp, which calls 0x004c5a60 at its original address.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
extern "C" int MashedRw_TextureDestroy_004c5a60(int tex);
#define MATCH_PORT 1
#define FUN_0040cf40 Orig_0040cf40
#define FUN_004c5a60 PortTop_0040cf40_Call004c5a60
extern "C" {
unsigned int PortTop_0040cf40_Call004c5a60(int p0) { return static_cast<unsigned int>(MashedRw_TextureDestroy_004c5a60(p0)); }
#include "../../../../re/match/s02/0040cf40.c"
}
#endif  // MASHED_STANDALONE
