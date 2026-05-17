// Mashed RE - Milestone B4 D3D9 textured-quad renderer.
//
// Uploads a single decoded TXD `Texture` (ARGB8888 only in B4) to a D3D9
// IDirect3DTexture9 with full mip chain, builds a screen-space quad vertex
// buffer (D3DFVF_XYZRHW | D3DFVF_TEX1) sized to the texture's base mip, and
// renders it centered in the back-buffer each frame.
//
// Deliberately scoped:
//   - ARGB8888 only (Paletted8 deferred to Milestone B5+).
//   - Screen-space (no view/projection matrices).
//   - Single texture; no batching, no sprite system.
//
// Owns the IDirect3DTexture9 + IDirect3DVertexBuffer9; caller owns the device.

#pragma once

#include <cstdint>

#include <d3d9.h>

#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace D3d9Render {

class QuadRenderer {
public:
    QuadRenderer() = default;
    ~QuadRenderer() { Shutdown(); }

    QuadRenderer(const QuadRenderer&)            = delete;
    QuadRenderer& operator=(const QuadRenderer&) = delete;

    // Bind to a device. Builds a default empty vertex buffer; no texture yet.
    bool Init(IDirect3DDevice9* device, int backbuffer_width, int backbuffer_height);

    // Upload mip 0..N from a decoded TXD texture. Supports ARGB8888
    // (lock+memcpy with stride translation + BGRA swizzle) and Paletted8
    // (CPU-expand each indexed byte through the per-texture RGBA palette into
    // a temporary BGRA row, then write into the locked rect). Returns false
    // on unsupported format or D3D9 failure. Updates the internal vertex
    // buffer to match the texture's base mip dimensions, centered.
    bool UploadFromTexture(const mashed_re::Txd::Texture& tex);

    // Issue Clear/BeginScene/EndScene already done by caller; we just set
    // states + DrawPrimitive. No-op if no texture uploaded.
    void Render();

    // Release D3D9 resources. Idempotent.
    void Shutdown();

    bool             has_texture() const { return m_tex != nullptr; }
    std::uint32_t    tex_width()  const  { return m_tex_w; }
    std::uint32_t    tex_height() const  { return m_tex_h; }
    const char*      last_error() const  { return m_last_error; }

private:
    bool BuildQuadVB(std::uint32_t tex_w, std::uint32_t tex_h);

    IDirect3DDevice9*       m_device      = nullptr;
    IDirect3DTexture9*      m_tex         = nullptr;
    IDirect3DVertexBuffer9* m_vb          = nullptr;
    int                     m_bb_width    = 0;
    int                     m_bb_height   = 0;
    std::uint32_t           m_tex_w       = 0;
    std::uint32_t           m_tex_h       = 0;
    const char*             m_last_error  = "uninitialised";
};

}  // namespace D3d9Render
}  // namespace mashed_re
