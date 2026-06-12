// Mashed RE — RW Im2D -> D3D9 bridge (Milestone B15, Phase F/H).
//
// The frontend's 2D draw reimpls (Frontend/DrawQuadPrimitives.cpp:
// HudIm2DQuad 0x00450b10, ChromeBaseDraw 0x00472c60, TextSprite* ...) are
// bit-identical C3 ports of MASHED's Im2D quad draws. Each one writes a shared
// 4-vertex buffer at DAT_00898a20 (4 verts x 0x1c bytes: X/Y/Z/W(rhw)/color/U/V
// = D3DFVF_XYZRHW|DIFFUSE|TEX1) and then dispatches through the RenderWare
// device object at *(0x007d3ff8):
//     z      = *(device + 0x18)          // RHW/Z source
//     setstate = (*(device + 0x20))(s,v) // RwRenderStateSet
//     draw     = (*(device + 0x30))(4, &DAT_00898a20, 4)  // RwIm2DRenderPrimitive
//
// In MASHED, *(0x007d3ff8) is the RenderWare 3 D3D driver device. In the
// standalone exe there is no RW engine, so that pointer is zero (wedge memory)
// and the draw reimpls would AV on the vtable call.
//
// This bridge stands up a FAKE RW device whose +0x20 / +0x30 slots point at our
// own handlers, and writes its address to *(0x007d3ff8). The draw handler reads
// the DAT_00898a20 vertex buffer and submits it to our IDirect3DDevice9 via
// DrawPrimitiveUP — so the unmodified C3 draw reimpls render correctly in the
// standalone with no change to their (verified) code.
//
// This is the Phase-F "stub renderer" (2D immediate-mode quads) the frontend
// menu needs; full RW3 is deferred to Phase 6+.

#pragma once

#include <cstdint>
#include <d3d9.h>

namespace mashed_re {
namespace D3d9Render {

// Install the fake RW device at *(0x007d3ff8) and bind it to `device` for
// submission. Must be called after the D3D9 device is created and after the
// Phase-G data wedge covers the 0x007d0000 granule. The write to 0x007d3ff8 is
// SEH-guarded by the caller convention (we VirtualQuery first). Returns true if
// the device pointer was installed.
bool RwIm2DBridge_Install(IDirect3DDevice9* device);

// Register a D3D9 texture under an integer handle. The frontend draw reimpls
// pass a texture handle as render-state 1 (HudIm2DQuad's tex_handle arg); the
// bridge resolves that handle -> IDirect3DTexture9* at draw time. Handle 0 is
// reserved for "no texture". Pass the same handle to HudIm2DQuad to draw textured.
void RwIm2DBridge_RegisterTexture(int handle, IDirect3DTexture9* texture);

// Mark a registered texture as POINT-sampled (default LINEAR). The FGDC20
// charset draws point-sampled in the original (FUN_00554940 render-state 9
// from the raster's native field; pixel-verified vs orig backbuffer dumps).
void RwIm2DBridge_SetTexturePointFilter(int handle, bool point);

// Diagnostics: count of DrawPrimitive submissions since install (for logging).
std::uint32_t RwIm2DBridge_DrawCount();

}  // namespace D3d9Render
}  // namespace mashed_re
