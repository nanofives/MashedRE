// Mashed RE — faithful FGDC20 font implementation (B19 faithful-glyph path).
// See MashedFont.h for the cracked on-disk formats.

#include "MashedFont.h"

#include <cstdlib>
#include <cstring>

#include "../Piz/PizReader.h"
#include "RwIm2DBridge.h"

namespace mashed_re {
namespace D3d9Render {

namespace {
inline std::uint32_t RdU32(const std::uint8_t* p, std::size_t o) {
    std::uint32_t v; std::memcpy(&v, p + o, 4); return v;
}
inline std::uint16_t RdU16(const std::uint8_t* p, std::size_t o) {
    std::uint16_t v; std::memcpy(&v, p + o, 2); return v;
}
inline float RdF32(const std::uint8_t* p, std::size_t o) {
    float v; std::memcpy(&v, p + o, 4); return v;
}
// Find a .piz entry by exact name; returns blob pointer + length or nullptr.
const std::uint8_t* FindBlob(mashed_re::Piz::Archive& piz, const char* name,
                             std::uint32_t* out_len) {
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        if (n && std::strcmp(n, name) == 0) return piz.blob(i, out_len);
    }
    return nullptr;
}
}  // namespace

bool MashedFont::Load(QuadRenderer& qr, std::uint32_t slot, int bridge_handle,
                      const char* piz_path) {
    m_ready = false;
    mashed_re::Piz::Archive piz;
    if (!piz.Load(piz_path)) return false;

    // ---- FGDC20.TXD: decode the 8bpp intensity atlas. ----
    std::uint32_t txd_len = 0;
    const std::uint8_t* txd = FindBlob(piz, "FGDC20.TXD", &txd_len);
    if (!txd || txd_len < 0x440) return false;
    if (RdU32(txd, 0x00) != 0x23) return false;       // root dict chunk
    // STRUCT payload @0x28 = [platform, w, h, depth].
    const std::uint32_t w     = RdU32(txd, 0x2c);
    const std::uint32_t h     = RdU32(txd, 0x30);
    const std::uint32_t depth = RdU32(txd, 0x34);
    if (w == 0 || h == 0 || w > 4096 || h > 4096 || depth != 8) return false;
    // 8bpp: a 1024-byte palette region (unused — atlas is direct intensity)
    // follows the 16-byte struct payload (@0x28..0x38), then w*h intensity bytes.
    const std::size_t pix_off = 0x38 + 1024;          // = 0x438
    if (pix_off + static_cast<std::size_t>(w) * h > txd_len) return false;
    const std::uint8_t* intensity = txd + pix_off;

    // Expand to BGRA: white text, alpha = intensity (so it alpha-blends).
    std::uint8_t* bgra = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(w) * h * 4));
    if (!bgra) return false;
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i) {
        bgra[i * 4 + 0] = 255;            // B
        bgra[i * 4 + 1] = 255;            // G
        bgra[i * 4 + 2] = 255;            // R
        bgra[i * 4 + 3] = intensity[i];   // A = coverage
    }
    const bool up = qr.UploadBGRAToSlot(slot, w, h, bgra);
    std::free(bgra);
    if (!up) return false;
    RwIm2DBridge_RegisterTexture(bridge_handle, qr.slot_texture(slot));
    m_handle = bridge_handle;
    m_atlasW = static_cast<float>(w);
    m_atlasH = static_cast<float>(h);

    // ---- FGDC20.RWF: char->glyph LUT + per-glyph UV rects. ----
    std::uint32_t rwf_len = 0;
    const std::uint8_t* rwf = FindBlob(piz, "FGDC20.RWF", &rwf_len);
    if (!rwf || rwf_len < 0x230) return false;
    m_height = RdF32(rwf, 0x14);                       // 33.0
    m_glyphCount = RdU32(rwf, 0x28);
    const std::uint32_t charTblCount = RdU32(rwf, 0x2c);
    if (m_glyphCount == 0 || m_glyphCount > 256) return false;
    const std::size_t lut_off   = 0x30 + static_cast<std::size_t>(charTblCount) * 2;
    const std::size_t glyph_off = lut_off + 0x100;
    if (glyph_off + static_cast<std::size_t>(m_glyphCount) * 21 > rwf_len) return false;

    // LUT: 128 u16 (chars 0..127) -> glyph index.
    for (int c = 0; c < 128; ++c) {
        m_lut[c] = RdU16(rwf, lut_off + static_cast<std::size_t>(c) * 2);
    }
    // Glyph UV records (21 bytes: 4 floats UV + int + page byte).
    for (std::uint32_t g = 0; g < m_glyphCount; ++g) {
        const std::size_t o = glyph_off + static_cast<std::size_t>(g) * 21;
        Glyf& gl = m_glyph[g];
        gl.u0 = RdF32(rwf, o + 0);
        gl.v0 = RdF32(rwf, o + 4);
        gl.u1 = RdF32(rwf, o + 8);
        gl.v1 = RdF32(rwf, o + 12);
        gl.w_px = (gl.u1 - gl.u0) * m_atlasW;
        gl.valid = (gl.u1 > gl.u0 && gl.v1 > gl.v0);
    }

    m_ready = true;
    return true;
}

bool MashedFont::Glyph(unsigned char ch, float uv[4], float* width_px) const {
    if (!m_ready || ch >= 128) return false;
    const std::uint16_t g = m_lut[ch];
    if (g >= m_glyphCount || !m_glyph[g].valid) return false;
    const Glyf& gl = m_glyph[g];
    uv[0] = gl.u0; uv[1] = gl.v0; uv[2] = gl.u1; uv[3] = gl.v1;
    if (width_px) *width_px = gl.w_px;
    return true;
}

}  // namespace D3d9Render
}  // namespace mashed_re
