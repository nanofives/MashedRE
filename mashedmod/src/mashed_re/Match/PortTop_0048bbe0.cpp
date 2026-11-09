// PORT-TOP-04G (L-96 top-down lane): exe-only port body of rva 0x0048bbe0 = the byte-matched source
// re/match/s14/0048bbe0.c (RwImageSetPath(0x005cc4de, an empty string), DAT_00722134 = RwTextureRead("sun" at
// 0x005cf4a8, 0); returns 1). Reached only on the MASHED_ORIG_TOP path, from the matched 0x0040bbb0
// (Match/PortTop_0040bbb0.cpp). "sun" comes from the knob-only rdata seed (Match/PortTop_RdataSeed.inc, row 0x005cf4a8);
// 0x005cc4de is a NUL byte in the original, the value the zero-filled exe mapping holds.
// The source passes an integer literal and the address of a pointer-typed global where it declares undefined4 * /
// undefined4: C accepts that, C++ does not. The source text is unchanged: the TU declares C++ overloads with those
// argument types, the calls resolve to them, and they forward to the RW bindings (LibRw/RwApiBind.cpp).
#ifdef MASHED_STANDALONE
#include <cstdint>
extern "C" int MashedRw_ImageSetPath_004ce2d0(const char* path);
extern "C" int MashedRw_TextureRead_004c5cb0(const char* name, const char* mask);
static inline unsigned int* PortTop0048bbe0_ImageSetPath(int p0) {
    return reinterpret_cast<unsigned int*>(static_cast<std::uintptr_t>(static_cast<unsigned>(
        MashedRw_ImageSetPath_004ce2d0(reinterpret_cast<const char*>(static_cast<std::uintptr_t>(static_cast<unsigned>(p0)))))));
}
static inline int PortTop0048bbe0_TextureRead(void** p0, int p1) {
    return MashedRw_TextureRead_004c5cb0(reinterpret_cast<const char*>(p0),
                                         reinterpret_cast<const char*>(static_cast<std::uintptr_t>(static_cast<unsigned>(p1))));
}
#define MATCH_PORT 1
#define FUN_0048bbe0 Orig_0048bbe0
#define FUN_004ce2d0 PortTop0048bbe0_ImageSetPath
#define FUN_004c5cb0 PortTop0048bbe0_TextureRead
extern "C" {
#include "../../../../re/match/s14/0048bbe0.c"
}
#endif  // MASHED_STANDALONE
