// Mashed RE — faithful FGDC20 font implementation (B19 faithful-glyph path).
// See MashedFont.h for the cracked on-disk formats.

#include "MashedFont.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

#include "../Piz/PizReader.h"
#include "RwIm2DBridge.h"

namespace mashed_re {
namespace D3d9Render {

namespace {
// Atlas supersampling: DISABLED (round-3). The standalone draws the 33px
// glyph cell at ~34 device px (800x600) — essentially 1:1, same as the
// original's ~0.82x at 640x480. The original simply uploads the raw FGDC20
// intensity atlas and bilinear-samples it; matching that gives the original's
// bold, smooth, solid glyphs. The 2x Catmull-Rom supersample was a mistake:
// its negative lobes (ringing) eroded thin strokes and gave the edges a
// ragged/noisy look (font_fair.png, round-3). Raw atlas + GPU LINEAR = match.
constexpr int kAtlasSS = 1;

inline float CatmullRom(float t) {
    t = t < 0 ? -t : t;
    if (t <= 1.0f) return 1.5f * t * t * t - 2.5f * t * t + 1.0f;
    if (t <  2.0f) return -0.5f * t * t * t + 2.5f * t * t - 4.0f * t + 2.0f;
    return 0.0f;
}

// Separable Catmull-Rom resample of an 8bpp coverage map to (w*K, h*K).
// Edge-clamped; result clamped 0..255 (CR overshoot = the sharpen).
std::uint8_t* UpscaleCoverage(const std::uint8_t* src, int w, int h, int K) {
    const int W = w * K, H = h * K;
    std::uint8_t* tmp = static_cast<std::uint8_t*>(std::malloc(
        static_cast<std::size_t>(W) * h));
    std::uint8_t* dst = static_cast<std::uint8_t*>(std::malloc(
        static_cast<std::size_t>(W) * H));
    if (!tmp || !dst) { std::free(tmp); std::free(dst); return nullptr; }
    auto clampi = [](int v, int lo, int hi) {
        return v < lo ? lo : (v > hi ? hi : v);
    };
    // Horizontal pass.
    for (int y = 0; y < h; ++y) {
        const std::uint8_t* row = src + static_cast<std::size_t>(y) * w;
        for (int X = 0; X < W; ++X) {
            const float sx = (X + 0.5f) / K - 0.5f;
            const int   ix = static_cast<int>(std::floor(sx));
            float acc = 0.f;
            for (int k = -1; k <= 2; ++k) {
                acc += CatmullRom(sx - (ix + k)) *
                       row[clampi(ix + k, 0, w - 1)];
            }
            const int v = static_cast<int>(acc + 0.5f);
            tmp[static_cast<std::size_t>(y) * W + X] =
                static_cast<std::uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v));
        }
    }
    // Vertical pass — PURE Catmull-Rom resample, NO edge hardening. Round-3
    // (font "pixelated, not smooth, same as before"): the DPI-1:1 fix removed
    // DWM's window-level smoothing that had been hiding the smoothstep
    // hardening's quantized edges. The original renders the FGDC20 atlas with
    // plain bilinear (smooth AA fringe); any thresholding here re-introduces
    // the jaggies. Keep the coverage map's smooth gradient intact — the GPU's
    // LINEAR minify then matches the original's anti-aliased look.
    for (int Y = 0; Y < H; ++Y) {
        const float sy = (Y + 0.5f) / K - 0.5f;
        const int   iy = static_cast<int>(std::floor(sy));
        for (int X = 0; X < W; ++X) {
            float acc = 0.f;
            for (int k = -1; k <= 2; ++k) {
                acc += CatmullRom(sy - (iy + k)) *
                       tmp[static_cast<std::size_t>(clampi(iy + k, 0, h - 1)) * W + X];
            }
            const int v = static_cast<int>(acc + 0.5f);
            dst[static_cast<std::size_t>(Y) * W + X] =
                static_cast<std::uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v));
        }
    }
    std::free(tmp);
    return dst;
}

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
    // Chunk layout (byte-exact, 2026-06-12): root 0x23 -> 4 raw bytes @0x0c ->
    // rwID_IMAGE (0x18) chunk @0x14 whose size 0x2041c = 12 (struct hdr) +
    // 0x10 (w,h,depth,STRIDE @0x2c..0x3b) + 0x400 (palette @0x3c..0x43b, all
    // zeros -> direct intensity) + 0x20000 (pixels). Pixels therefore start at
    // 0x43c — the previous 0x438 read the palette's last dword as the first
    // four pixels, shifting the WHOLE atlas 4 columns left (every glyph showed
    // its neighbor's edge column and lost its rightmost columns = the
    // long-standing "leading tick" + cut-off glyph edges).
    const std::size_t pix_off = 0x3c + 1024;          // = 0x43c
    if (pix_off + static_cast<std::size_t>(w) * h > txd_len) return false;
    const std::uint8_t* intensity = txd + pix_off;

    // Supersample the coverage map (see kAtlasSS), then expand to BGRA:
    // white text, alpha = coverage (so it alpha-blends).
    const std::uint32_t W = w * kAtlasSS, H = h * kAtlasSS;
    std::uint8_t* cov = UpscaleCoverage(intensity,
                                        static_cast<int>(w),
                                        static_cast<int>(h), kAtlasSS);
    if (!cov) return false;
    std::uint8_t* bgra = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(W) * H * 4));
    if (!bgra) { std::free(cov); return false; }
    for (std::size_t i = 0; i < static_cast<std::size_t>(W) * H; ++i) {
        bgra[i * 4 + 0] = 255;            // B
        bgra[i * 4 + 1] = 255;            // G
        bgra[i * 4 + 2] = 255;            // R
        bgra[i * 4 + 3] = cov[i];         // A = coverage
    }
    std::free(cov);
    const bool up = qr.UploadBGRAToSlot(slot, W, H, bgra);
    std::free(bgra);
    if (!up) return false;
    RwIm2DBridge_RegisterTexture(bridge_handle, qr.slot_texture(slot));
    // LINEAR sampling (bridge default). The menu drawer FUN_00428140 sets
    // render-state 9 = 2 (rwFILTERLINEAR) before printing; FUN_00554940's
    // per-raster override value (*(texture+0x50)&0xff) is [UNCERTAIN] without
    // a runtime read. The earlier POINT switch was compensating for the
    // 4-column atlas shift (pixels @0x438 instead of 0x43c) — with the
    // correct base, LINEAR renders clean solid glyphs and at the original's
    // 640x480 (cell scale 1.03x) LINEAR and POINT are visually identical.
    m_handle = bridge_handle;
    m_atlasW = static_cast<float>(w);
    m_atlasH = static_cast<float>(h);

    // ---- FGDC20.RWF: char->glyph LUT + per-glyph UV rects. ----
    std::uint32_t rwf_len = 0;
    const std::uint8_t* rwf = FindBlob(piz, "FGDC20.RWF", &rwf_len);
    if (!rwf || rwf_len < 0x230) return false;
    // Header (12-byte chunk hdr + version dword, then the loader's read order
    // FUN_00554390: cs+0x00 type, +0x04, +0x08, +0x0c, +0x10 flags, +0x124
    // ext_base, +0x130 count, +0x128 ext_count):
    m_height   = RdF32(rwf, 0x14);                     // cs+0x04 = 33.0
    m_c8       = RdF32(rwf, 0x18);                     // cs+0x08 = 0.151515 (5/33)
    m_tracking = RdF32(rwf, 0x1c);                     // cs+0x0c = 0.0
    m_extBase    = RdU32(rwf, 0x24);                   // FGDC20: 0x80
    m_glyphCount = RdU32(rwf, 0x28);                   // FGDC20: 225
    m_extCount   = RdU32(rwf, 0x2c);                   // FGDC20: 128
    if (m_glyphCount == 0 || m_glyphCount > 256) return false;
    if (m_extCount > 256) return false;
    const std::size_t lut_off   = 0x30 + static_cast<std::size_t>(m_extCount) * 2;
    const std::size_t glyph_off = lut_off + 0x100;
    if (glyph_off + static_cast<std::size_t>(m_glyphCount) * 21 > rwf_len) return false;

    // Extended codepoint -> glyph table (FUN_00554390 reads it BEFORE the
    // ASCII table: ext-count u16s into font+0x12c; covers codepoints
    // ext_base..ext_base+ext_count-1 — the 0x80..0x8f nav glyphs live here).
    for (std::uint32_t c = 0; c < m_extCount; ++c) {
        m_ext[c] = static_cast<std::int16_t>(
            RdU16(rwf, 0x30 + static_cast<std::size_t>(c) * 2));
    }
    // ASCII LUT: 128 i16 (chars 0..127) -> glyph index, -1 = no glyph
    // (FUN_00554940 reads SIGNED shorts from font+0x24: `(int)*(short*)...`).
    for (int c = 0; c < 128; ++c) {
        m_lut[c] = static_cast<std::int16_t>(
            RdU16(rwf, lut_off + static_cast<std::size_t>(c) * 2));
    }
    // Glyph records, 21 bytes on disk: [4 UV floats][width float][page byte]
    // (FUN_00554390 reads the 16 UV bytes to rec+0x08, the width dword to
    // rec+0x00, the page byte to rec+0x1c). The width float is the glyph
    // width as a fraction of the unit cell — FUN_00554940 uses it for the
    // quad width AND the pen advance. For FGDC20 it equals the atlas u-extent
    // exactly ((u1-u0)*512 == width*33 for every record).
    for (std::uint32_t g = 0; g < m_glyphCount; ++g) {
        const std::size_t o = glyph_off + static_cast<std::size_t>(g) * 21;
        Glyf& gl = m_glyph[g];
        gl.u0 = RdF32(rwf, o + 0);
        gl.v0 = RdF32(rwf, o + 4);
        gl.u1 = RdF32(rwf, o + 8);
        gl.v1 = RdF32(rwf, o + 12);
        gl.w_frac = RdF32(rwf, o + 16);
        // (No UV adjustment: with the correct 0x43c pixel base every glyph
        // sits inside its .5-boundary window with clean margins — the old
        // half-texel inset compensated for the 4-column atlas shift.)
        // The space record has a real width with a (blank) UV rect — the
        // original draws it like any glyph. Validity = a usable rect.
        gl.valid = (gl.u1 >= gl.u0 && gl.v1 > gl.v0);
    }

    m_ready = true;
    return true;
}

bool MashedFont::Glyph(unsigned char ch, float uv[4], float* width_frac) const {
    if (!m_ready) return false;
    std::int16_t g;
    if (ch < 128) {
        g = m_lut[ch];
    } else if (ch >= m_extBase &&
               static_cast<std::uint32_t>(ch) < m_extBase + m_extCount) {
        g = m_ext[ch - m_extBase];   // extended table (nav glyphs 0x80..0x8f)
    } else {
        return false;
    }
    if (g < 0 || static_cast<std::uint32_t>(g) >= m_glyphCount ||
        !m_glyph[g].valid)
        return false;
    const Glyf& gl = m_glyph[g];
    uv[0] = gl.u0; uv[1] = gl.v0; uv[2] = gl.u1; uv[3] = gl.v1;
    if (width_frac) *width_frac = gl.w_frac;
    return true;
}

}  // namespace D3d9Render
}  // namespace mashed_re
