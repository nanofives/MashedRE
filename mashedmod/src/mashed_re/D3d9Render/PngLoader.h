// Mashed RE — WIC-based image decoder (B19a).
//
// The frontend's background and logo are PNGs (Perm.piz/BACKGROUND.PNG,
// Font36.piz/MASHEDLOGO.PNG) — not the RenderWare TXD format that TxdDecoder
// handles. This decodes them via Windows Imaging Component (WIC), a native
// Win32 facility (no third-party dependency), into a 32bpp BGRA buffer ready to
// upload to a D3DFMT_A8R8G8B8 texture.
#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace D3d9Render {

// Decode an image (PNG/BMP/etc. — anything WIC supports) from an in-memory blob
// into a freshly-malloc'd top-down 32bpp BGRA buffer (stride = w*4). Returns the
// buffer (caller frees via std::free) and writes the dimensions; returns nullptr
// on failure. BGRA byte order matches D3DFMT_A8R8G8B8, so no swizzle is needed.
std::uint8_t* DecodeImageToBGRA(const std::uint8_t* data, std::size_t len,
                                std::uint32_t* out_w, std::uint32_t* out_h);

}  // namespace D3d9Render
}  // namespace mashed_re
