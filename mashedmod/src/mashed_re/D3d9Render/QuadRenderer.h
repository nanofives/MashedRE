// Mashed RE - Milestone B4/B5 D3D9 textured-quad renderer.
//
// B4: Uploads a single decoded TXD `Texture` (ARGB8888 + Paletted8) to a D3D9
// IDirect3DTexture9 with full mip chain and renders it as a centered
// screen-space quad sized to its base mip.
//
// B5: Extends the renderer to host up to N (kMaxSlots) decoded TXD textures
// simultaneously. Each slot owns its own IDirect3DTexture9. RenderAt(slot,
// cx, cy, w, h) draws the chosen slot's texture at an arbitrary screen-space
// rectangle (rebuilds the shared vertex buffer per call). Used by exe_main.cpp
// to lay out all 8 Frontend.piz textures as a 4x2 atlas in the 800x600 window.
//
// Deliberately scoped:
//   - ARGB8888 + Paletted8 (CPU-expanded to BGRA on upload).
//   - Screen-space (no view/projection matrices).
//   - No batching or sprite system.
//
// Owns the IDirect3DTexture9 array + IDirect3DVertexBuffer9; caller owns the
// device.

#pragma once

#include <cstdint>

#include <d3d9.h>

#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace D3d9Render {

class QuadRenderer {
public:
    // Hard cap on atlas slots. Frontend.piz contains 8 textures so 16 is a
    // comfortable upper bound for B5.
    static constexpr std::uint32_t kMaxSlots = 16;

    QuadRenderer() = default;
    ~QuadRenderer() { Shutdown(); }

    QuadRenderer(const QuadRenderer&)            = delete;
    QuadRenderer& operator=(const QuadRenderer&) = delete;

    // Bind to a device. Builds a default empty vertex buffer; no textures yet.
    bool Init(IDirect3DDevice9* device, int backbuffer_width, int backbuffer_height);

    // B4 single-texture entry point (kept for compatibility). Uploads into
    // slot 0 and rebuilds the shared VB sized to the texture's base mip
    // centered in the backbuffer. Render() then draws slot 0 centered.
    bool UploadFromTexture(const mashed_re::Txd::Texture& tex);

    // B5 multi-slot entry point. Uploads `tex` into slot `slot` (0..kMaxSlots-1).
    // Does NOT touch the shared VB — callers use RenderAt() afterwards which
    // builds a per-call VB with explicit screen coords. Returns false if slot
    // is out of range, format is unsupported, or D3D9 calls fail.
    bool UploadFromTextureToSlot(std::uint32_t slot,
                                 const mashed_re::Txd::Texture& tex);

    // B4 path: draw slot 0 at the centered layout produced by the last
    // UploadFromTexture() call. No-op if no slot-0 texture present.
    void Render();

    // B5 path: draw slot `slot` at screen-space rectangle centered on
    // (cx, cy) with size (w, h). Rebuilds the shared VB each call (the cost
    // is negligible — 4 verts, 96 bytes). No-op if slot has no texture.
    void RenderAt(std::uint32_t slot, float cx, float cy, float w, float h);

    // Release D3D9 resources. Idempotent.
    void Shutdown();

    bool             has_texture() const { return m_textures[0] != nullptr; }
    bool             has_slot(std::uint32_t slot) const {
        return slot < kMaxSlots && m_textures[slot] != nullptr;
    }
    std::uint32_t    tex_width()  const  { return m_tex_w[0]; }
    std::uint32_t    tex_height() const  { return m_tex_h[0]; }
    const char*      last_error() const  { return m_last_error; }

private:
    bool BuildQuadVBCentered(std::uint32_t tex_w, std::uint32_t tex_h);
    bool BuildQuadVBAt(float cx, float cy, float w, float h);
    bool UploadIntoTextureSlot(std::uint32_t slot,
                               const mashed_re::Txd::Texture& tex);
    void SetCommonRenderStates();

    IDirect3DDevice9*       m_device      = nullptr;
    IDirect3DTexture9*      m_textures[kMaxSlots] = {};
    std::uint32_t           m_tex_w[kMaxSlots]    = {};
    std::uint32_t           m_tex_h[kMaxSlots]    = {};
    IDirect3DVertexBuffer9* m_vb          = nullptr;
    int                     m_bb_width    = 0;
    int                     m_bb_height   = 0;
    const char*             m_last_error  = "uninitialised";
};

}  // namespace D3d9Render
}  // namespace mashed_re
