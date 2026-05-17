// Mashed RE - Milestone B4 D3D9 textured-quad renderer.
// See QuadRenderer.h for scope. Implementation notes:
//
// * Vertex format: D3DFVF_XYZRHW | D3DFVF_TEX1. XYZRHW lets us skip the
//   world/view/proj pipeline entirely — coordinates are post-transform
//   screen-space pixels. rhw=1.0 disables perspective divide.
//
// * Triangle strip layout (4 verts):
//      0 top-left  -- 1 top-right
//                       |  /  |
//      2 bot-left  -- 3 bot-right
//   Winding (0->1->2->3) yields two CCW triangles; we set CULL_NONE so
//   winding doesn't matter, but the order is conventional.
//
// * Byte-order swizzle: TXD's payload (per re/analysis/txd_on_disk_format)
//   is RW3-native RGBA in memory. D3DFMT_A8R8G8B8 expects BGRA in memory
//   (with A in the high byte when read as a uint32 little-endian). So
//   per-pixel we swap src[R] <-> src[B] into the locked rect. The
//   call-site in exe_main.cpp checks the screenshot afterwards; if colors
//   look swapped this swizzle is where to flip them.
//
// * Mip chain: B3's decoder pre-builds 7-9 mips per Frontend texture; we
//   pass them all to CreateTexture (Levels=mip_count) and LockRect each.

#include "QuadRenderer.h"

#include <cstring>

namespace mashed_re {
namespace D3d9Render {

namespace {

struct Vert {
    float x, y, z, rhw;
    float u, v;
};

constexpr DWORD kVertFvf = D3DFVF_XYZRHW | D3DFVF_TEX1;

template <typename T>
inline void SafeRelease(T*& p) {
    if (p) { p->Release(); p = nullptr; }
}

}  // namespace

bool QuadRenderer::Init(IDirect3DDevice9* device, int bb_w, int bb_h) {
    if (!device) { m_last_error = "null device"; return false; }
    m_device      = device;
    m_bb_width    = bb_w;
    m_bb_height   = bb_h;
    m_last_error  = "ok";
    return true;
}

void QuadRenderer::Shutdown() {
    SafeRelease(m_tex);
    SafeRelease(m_vb);
    m_device = nullptr;
}

bool QuadRenderer::BuildQuadVB(std::uint32_t tex_w, std::uint32_t tex_h) {
    SafeRelease(m_vb);

    // Center the quad in the backbuffer. If the texture is larger than the
    // backbuffer (it isn't for any Frontend.piz B3 texture, max 256x256), it
    // will be clipped — acceptable for B4.
    const float quad_w = static_cast<float>(tex_w);
    const float quad_h = static_cast<float>(tex_h);
    const float x0     = (static_cast<float>(m_bb_width)  - quad_w) * 0.5f;
    const float y0     = (static_cast<float>(m_bb_height) - quad_h) * 0.5f;
    const float x1     = x0 + quad_w;
    const float y1     = y0 + quad_h;

    // D3D9 pixel-center convention requires a -0.5 offset for crisp 1:1
    // texel-to-pixel mapping with XYZRHW. Without it the quad samples halfway
    // between neighbouring texels and looks softened.
    const float ox = -0.5f;
    const float oy = -0.5f;

    Vert verts[4] = {
        // x,             y,             z,   rhw,  u,    v
        { x0 + ox,        y0 + oy,       0.f, 1.f,  0.f,  0.f },  // 0 TL
        { x1 + ox,        y0 + oy,       0.f, 1.f,  1.f,  0.f },  // 1 TR
        { x0 + ox,        y1 + oy,       0.f, 1.f,  0.f,  1.f },  // 2 BL
        { x1 + ox,        y1 + oy,       0.f, 1.f,  1.f,  1.f },  // 3 BR
    };

    HRESULT hr = m_device->CreateVertexBuffer(
        sizeof(verts),
        D3DUSAGE_WRITEONLY,
        kVertFvf,
        D3DPOOL_MANAGED,
        &m_vb,
        nullptr);
    if (FAILED(hr) || !m_vb) {
        m_last_error = "CreateVertexBuffer failed";
        return false;
    }

    void* locked = nullptr;
    hr = m_vb->Lock(0, sizeof(verts), &locked, 0);
    if (FAILED(hr) || !locked) {
        m_last_error = "VB Lock failed";
        SafeRelease(m_vb);
        return false;
    }
    std::memcpy(locked, verts, sizeof(verts));
    m_vb->Unlock();
    return true;
}

bool QuadRenderer::UploadFromTexture(const mashed_re::Txd::Texture& tex) {
    if (!m_device) { m_last_error = "not initialised"; return false; }
    if (tex.mip_count == 0) { m_last_error = "texture has no mips"; return false; }

    const auto fmt = tex.format();
    if (fmt != mashed_re::Txd::PixelFormat::ARGB8888 &&
        fmt != mashed_re::Txd::PixelFormat::Paletted8) {
        m_last_error = "unsupported format (need ARGB8888 or Paletted8)";
        return false;
    }
    if (fmt == mashed_re::Txd::PixelFormat::Paletted8 && !tex.mips[0].palette) {
        m_last_error = "PAL8 texture has no palette";
        return false;
    }

    SafeRelease(m_tex);

    const std::uint32_t base_w = tex.mips[0].width;
    const std::uint32_t base_h = tex.mips[0].height;
    const UINT          levels = static_cast<UINT>(tex.mip_count);

    // We always create an A8R8G8B8 D3D9 texture (D3DFMT_P8 is unsupported on
    // modern drivers). PAL8 sources get CPU-expanded into ARGB8888 on the
    // way to LockRect.
    HRESULT hr = m_device->CreateTexture(
        base_w, base_h, levels,
        0,                       // usage
        D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED,
        &m_tex,
        nullptr);
    if (FAILED(hr) || !m_tex) {
        m_last_error = "CreateTexture failed";
        return false;
    }

    for (std::uint32_t level = 0; level < tex.mip_count; ++level) {
        const auto& mip = tex.mips[level];

        D3DLOCKED_RECT lr;
        hr = m_tex->LockRect(level, &lr, nullptr, 0);
        if (FAILED(hr)) {
            m_last_error = "LockRect failed";
            SafeRelease(m_tex);
            return false;
        }

        const std::uint8_t* src     = mip.pixels;
        std::uint8_t*       dst_row = static_cast<std::uint8_t*>(lr.pBits);

        if (fmt == mashed_re::Txd::PixelFormat::ARGB8888) {
            for (std::uint32_t y = 0; y < mip.height; ++y) {
                const std::uint8_t* src_row = src + static_cast<std::size_t>(y) * mip.stride;
                for (std::uint32_t x = 0; x < mip.width; ++x) {
                    // TXD: src[0..3] = R G B A (RW3-native)
                    // D3D9 A8R8G8B8 reads dst as little-endian uint32 = 0xAARRGGBB,
                    // so in memory the byte order is B G R A.
                    const std::uint8_t r = src_row[x * 4 + 0];
                    const std::uint8_t g = src_row[x * 4 + 1];
                    const std::uint8_t b = src_row[x * 4 + 2];
                    const std::uint8_t a = src_row[x * 4 + 3];
                    dst_row[x * 4 + 0] = b;
                    dst_row[x * 4 + 1] = g;
                    dst_row[x * 4 + 2] = r;
                    dst_row[x * 4 + 3] = a;
                }
                dst_row += lr.Pitch;
            }
        } else {
            // Paletted8: each src byte is an index into mip.palette, which is
            // 256 entries of 4 bytes RGBA. Expand to BGRA into the locked
            // rect. Palette is per-mip in the TXD format (per the B3 spec)
            // so we read mip.palette here, not tex.mips[0].palette.
            const std::uint8_t* pal = mip.palette;
            if (!pal) {
                // Mip without its own palette: fall back to the base mip's.
                pal = tex.mips[0].palette;
            }
            if (!pal) {
                m_last_error = "PAL8 mip missing palette";
                m_tex->UnlockRect(level);
                SafeRelease(m_tex);
                return false;
            }
            for (std::uint32_t y = 0; y < mip.height; ++y) {
                const std::uint8_t* src_row = src + static_cast<std::size_t>(y) * mip.stride;
                for (std::uint32_t x = 0; x < mip.width; ++x) {
                    const std::uint8_t idx = src_row[x];
                    const std::uint8_t r = pal[idx * 4 + 0];
                    const std::uint8_t g = pal[idx * 4 + 1];
                    const std::uint8_t b = pal[idx * 4 + 2];
                    const std::uint8_t a = pal[idx * 4 + 3];
                    dst_row[x * 4 + 0] = b;
                    dst_row[x * 4 + 1] = g;
                    dst_row[x * 4 + 2] = r;
                    dst_row[x * 4 + 3] = a;
                }
                dst_row += lr.Pitch;
            }
        }
        m_tex->UnlockRect(level);
    }

    m_tex_w = base_w;
    m_tex_h = base_h;

    if (!BuildQuadVB(base_w, base_h)) {
        // BuildQuadVB sets m_last_error
        SafeRelease(m_tex);
        return false;
    }

    m_last_error = "ok";
    return true;
}

void QuadRenderer::Render() {
    if (!m_device || !m_tex || !m_vb) return;

    m_device->SetRenderState(D3DRS_LIGHTING,        FALSE);
    m_device->SetRenderState(D3DRS_ZENABLE,         FALSE);
    m_device->SetRenderState(D3DRS_CULLMODE,        D3DCULL_NONE);
    m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    m_device->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
    m_device->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

    m_device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
    m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    // Disable stage 1 so it doesn't multiply by something undefined.
    m_device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
    m_device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

    m_device->SetTexture(0, m_tex);
    m_device->SetFVF(kVertFvf);
    m_device->SetStreamSource(0, m_vb, 0, sizeof(Vert));
    m_device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

}  // namespace D3d9Render
}  // namespace mashed_re
