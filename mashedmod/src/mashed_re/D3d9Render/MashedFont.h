// Mashed RE — faithful FGDC20 font (B19 faithful-glyph path).
//
// Decodes MASHED's actual menu font from Font36.piz:
//   FGDC20.TXD — 512x256 8bpp direct-intensity glyph atlas (RW TXD variant
//                ver 0x1c02000a: root 0x23, struct @0x28 = [plat,w,h,depth],
//                1024-byte palette region, then w*h intensity bytes @0x438).
//   FGDC20.RWF — RW chunk 0x199 (RtCharset). Cracked from the deserializer
//                FUN_00554390: header [ext_base u32 @0x24 (=0x80), glyphCount
//                u32 @0x28 (=225), ext_count u32 @0x2c (=128)], EXTENDED
//                codepoint->glyph table (ext_count u16 @0x30; covers chars
//                ext_base..ext_base+ext_count-1 — the prompt-strip nav glyphs
//                0x80..0x8f live here), ASCII char->glyph LUT (128 u16
//                @0x130), then `glyphCount` 21-byte glyph records @0x230,
//                each [u0,v0,u1,v1 floats][int][page byte]. Struct mirror:
//                font+0x124 ext-base / +0x128 ext-count / +0x12c ext-table /
//                +0x24 i16[128] ASCII table (FUN_00554390 read order).
//
// The atlas is uploaded as a white-text/alpha texture and registered with the
// RwIm2DBridge; the caller draws each glyph as a UV-rect quad via HudIm2DQuad.
#pragma once

#include <cstdint>
#include "QuadRenderer.h"

namespace mashed_re {
namespace D3d9Render {

class MashedFont {
public:
    // Load + decode FGDC20.{TXD,RWF} from `piz_path`, upload the atlas into
    // QuadRenderer slot `slot`, and register it with the bridge under
    // `bridge_handle`. Returns true on success.
    bool Load(QuadRenderer& qr, std::uint32_t slot, int bridge_handle,
              const char* piz_path);

    bool  ready()          const { return m_ready; }
    int   handle()         const { return m_handle; }
    float natural_height() const { return m_height; }   // glyph cell height, px

    // Look up a char's glyph: ASCII (0..127) via the LUT, extended codepoints
    // (ext_base..ext_base+ext_count-1, e.g. the 0x80..0x8f nav glyphs) via the
    // extended table. Fills uv[4] = {u0,v0,u1,v1} and *width_px (the glyph's
    // atlas width in pixels = advance). Returns false if the char has no glyph
    // (caller advances by a space).
    bool Glyph(unsigned char ch, float uv[4], float* width_px) const;

private:
    bool          m_ready   = false;
    int           m_handle  = 0;
    float         m_atlasW  = 512.f;
    float         m_atlasH  = 256.f;
    float         m_height  = 33.f;
    std::uint32_t m_glyphCount = 0;
    struct Glyf { float u0, v0, u1, v1, w_px; bool valid; };
    Glyf          m_glyph[256] = {};
    std::uint16_t m_lut[128]   = {};   // char (0..127) -> glyph index
    std::uint32_t m_extBase  = 0;      // font+0x124 (FGDC20: 0x80)
    std::uint32_t m_extCount = 0;      // font+0x128 (FGDC20: 128)
    std::uint16_t m_ext[256]   = {};   // font+0x12c: (ch-ext_base) -> glyph
};

}  // namespace D3d9Render
}  // namespace mashed_re
