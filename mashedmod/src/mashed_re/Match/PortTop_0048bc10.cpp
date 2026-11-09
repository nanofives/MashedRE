// PORT-TOP-04H (item 3): exe-only port body of rva 0x0048bc10 = the byte-matched source re/match/s14/0048bc10.c
// (match.py BYTE-IDENTICAL 13 B): RwTextureDestroy(DAT_00722134), the "sun" texture 0x0048bbe0 reads
// (Match/PortTop_0048bbe0.cpp). Reached only on the MASHED_ORIG_TOP path, from the matched AppDestroy callee 0x0040bd00
// (Match/PortTop_0040bd00.cpp), which calls it before its four RwTexDictionaryDestroy calls. The texture is freed there:
// RwTextureDestroy drops the count to 0, unlinks it from its dictionary and frees it (re/match/rwl01/004c5a60.c; librw
// rw::Texture::destroy does the same), so the dictionary destroys that follow do not see it again.
// The source passes the undefined4 global where it declares int *: C accepts that, C++ does not. The source text is
// unchanged: the TU declares a C++ overload with that argument type, which forwards to the RW binding (LibRw/RwApiBind.cpp).
#ifdef MASHED_STANDALONE
extern "C" int MashedRw_TextureDestroy_004c5a60(int tex);
static inline unsigned int PortTop0048bc10_TextureDestroy(unsigned int tex) {
    return static_cast<unsigned int>(MashedRw_TextureDestroy_004c5a60(static_cast<int>(tex)));
}
#define MATCH_PORT 1
#define FUN_0048bc10 Orig_0048bc10
#define FUN_004c5a60 PortTop0048bc10_TextureDestroy
extern "C" {
#include "../../../../re/match/s14/0048bc10.c"
}
#endif  // MASHED_STANDALONE
