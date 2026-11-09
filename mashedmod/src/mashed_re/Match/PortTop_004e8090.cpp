// PORT-TOP-04I (PTank family): exe-only port body of rva 0x004e8090 (RpMaterialSetTexture) = the matched source
// re/match/rwl02/004e8090.c: raises the new texture's count (+0x54), destroys the old texture (RwTextureDestroy
// 0x004c5a60) when there is one, stores the new one at material +0x00; returns the material. Callers on the
// MASHED_ORIG_TOP path: 0x004881d0 / 0x004880a0 on the PTank record's material view, with the FX dictionary's
// "lensflare-fs8" texture (a librw rw::Texture: its count is at +0x54 too, LibRw/RwApiBind.cpp static_assert).
// The source declares the callee with an int * argument, the binding takes an int: this TU defines that C prototype as a
// forwarder to the binding.
#ifdef MASHED_STANDALONE
#include <cstdint>
extern "C" int MashedRw_TextureDestroy_004c5a60(int tex);
extern "C" unsigned int PortTop004e8090_TextureDestroy(int* tex) {
    return static_cast<unsigned int>(
        MashedRw_TextureDestroy_004c5a60(static_cast<int>(reinterpret_cast<std::uintptr_t>(tex))));
}
#define MATCH_PORT 1
#define FUN_004e8090 Orig_004e8090
#define FUN_004c5a60 PortTop004e8090_TextureDestroy
extern "C" {
#include "../../../../re/match/rwl02/004e8090.c"
}
#endif  // MASHED_STANDALONE
