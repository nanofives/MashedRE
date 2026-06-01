// Mashed RE — GDI text → BGRA rasterizer (B19b).
//
// The frontend's menu item strings are plain [u16 length][UTF-16LE text] records
// in the language .DAT files (e.g. Font36.piz/USA.DAT) — real text like
// "Single Player" / "Options". This rasterizes such a string via GDI into a
// 32bpp BGRA buffer (white text, per-pixel alpha = glyph coverage) ready to
// upload to a D3DFMT_A8R8G8B8 texture and draw through the Im2D bridge.
//
// This renders the real game text in a system font; reproducing MASHED's exact
// FGDC20 glyph atlas (Font36.piz/FGDC20.TXD + .RWF metrics) is a future fidelity
// refinement — the content here is faithful, the typeface is not.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace D3d9Render {

// Rasterize a UTF-16 string into a freshly-malloc'd top-down 32bpp BGRA buffer:
// RGB = white, alpha = text coverage, transparent elsewhere. Caller frees via
// std::free. Returns nullptr on failure; writes the bitmap dimensions.
std::uint8_t* RenderTextToBGRA(const wchar_t* text, int font_px,
                               std::uint32_t* out_w, std::uint32_t* out_h);

}  // namespace D3d9Render
}  // namespace mashed_re
