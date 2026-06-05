// Mashed RE — Frontend Im2D draw-quad primitives (c3-batch-af-s1 harvest).
//
// Two more siblings of the DrawQuadPrimitives.cpp family: scalar-signature
// RwIm2D quad renderers that build the shared 4-vertex buffer at DAT_00898a20
// (4 verts x 28 bytes) then dispatch through the RW driver vtable at
// DAT_007d3ff8 (+0x18 RHW source, +0x20 SetRenderState, +0x30 DrawPrimitive).
//
// Verified bit-identical via Frida draw_quad_observe (post-call 112-byte vertex
// buffer fingerprint, orig vs reimpl, same live vtable).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00473540  GradientQuadHorizAlpha — 5-arg quad; per-vertex color is a
//                                        horizontal alpha split: V0/V2 (left
//                                        column) keep param_5's alpha byte,
//                                        V1/V3 (right column) force alpha = 0.
//   0x004736c0  BorderQuadFourAlpha    — 9-arg quad; four independent per-vertex
//                                        alpha bytes (param_6..param_9) combined
//                                        with the shared swapped RGB of param_5.
//
// Analysis notes:
//   re/analysis/hud_frontend/0x00473540.md
//   re/analysis/font_pools_frontend_ae6/0x004736c0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Shared layout — 4-vertex buffer DAT_00898a20..DAT_00898a98
// 4 vertices x 7 dwords (28 bytes) each:
//   +0x00 X  +0x04 Y  +0x08 Z(RHW)  +0x0c W  +0x10 color  +0x14 U  +0x18 V
// ---------------------------------------------------------------------------
namespace {

constexpr std::uintptr_t kVBufBase     = 0x00898a20u;
constexpr std::uintptr_t kVtableGlobal = 0x007d3ff8u;
constexpr std::size_t    kVertStride   = 0x1cu;       // 28 bytes per vertex

constexpr std::uintptr_t kV0 = kVBufBase + 0 * kVertStride;  // 0x00898a20
constexpr std::uintptr_t kV1 = kVBufBase + 1 * kVertStride;  // 0x00898a3c
constexpr std::uintptr_t kV2 = kVBufBase + 2 * kVertStride;  // 0x00898a58
constexpr std::uintptr_t kV3 = kVBufBase + 3 * kVertStride;  // 0x00898a74

// Scale globals (cited at each function's body).
constexpr std::uintptr_t kScaleX = 0x005cd5a8u;  // _DAT_005cd5a8  X screen-scale
constexpr std::uintptr_t kScaleY = 0x005cc560u;  // _DAT_005cc560  Y screen-scale

// Screen-dimension getters (FUN_0042b8b0 / FUN_0042b8c0).
using ShortGetter_t = std::uint16_t(__cdecl*)();
auto* const s_ScreenWidthGet  = reinterpret_cast<ShortGetter_t>(0x0042b8b0);
auto* const s_ScreenHeightGet = reinterpret_cast<ShortGetter_t>(0x0042b8c0);

using RwSetState_t = void(__cdecl*)(int, int);
using RwDrawPrim_t = void(__cdecl*)(int, void*, int);

static inline std::uintptr_t vtable_base() {
    return *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
}
static inline void rw_set_state(int a, int b) {
    (*reinterpret_cast<RwSetState_t*>(vtable_base() + 0x20))(a, b);
}
static inline void rw_draw_4verts() {
    (*reinterpret_cast<RwDrawPrim_t*>(vtable_base() + 0x30))(
        4, reinterpret_cast<void*>(kVBufBase), 4);
}
// Z field source = *(vtable+0x18); written to each vertex +0x08 in the loop.
static inline std::uint32_t rw_z_field() {
    return *reinterpret_cast<std::uint32_t*>(vtable_base() + 0x18);
}

// X/Y writers (matches the TL/TR/BL/BR ordering used by both functions:
// V0=(x,y), V1=(x+w,y), V2=(x,y+h), V3=(x+w,y+h) — same as ChromeBaseDraw).
static inline void write_vert_xy(std::uintptr_t vb, float x, float y) {
    *reinterpret_cast<float*>(vb + 0x00) = x;
    *reinterpret_cast<float*>(vb + 0x04) = y;
}
static inline void fill_xy_quad(float x, float y, float w, float h) {
    write_vert_xy(kV0, x,     y    );
    write_vert_xy(kV1, x + w, y    );
    write_vert_xy(kV2, x,     y + h);
    write_vert_xy(kV3, x + w, y + h);
}

// Per-vertex Z(RHW)/W writer — the do/while loop in both functions writes
// puVar[-1] = *(vtable+0x18) (Z) and *puVar = 0x3f800000 (W=1.0f) for V0..V3.
static inline void write_vert_zw(std::uintptr_t vb, std::uint32_t z) {
    *reinterpret_cast<std::uint32_t*>(vb + 0x08) = z;            // Z (RHW)
    *reinterpret_cast<std::uint32_t*>(vb + 0x0c) = 0x3f800000u;  // W = 1.0f
}
static inline void fill_zw_all(std::uint32_t z) {
    write_vert_zw(kV0, z);
    write_vert_zw(kV1, z);
    write_vert_zw(kV2, z);
    write_vert_zw(kV3, z);
}

// R<->B swap of param_5's low 24 bits: (B<<16)|(G<<8)|R. This is the shared
// "rgb_low" both functions OR with a per-vertex alpha byte. Equivalent to
// color_swap_argb_to_abgr(argb) & 0x00ffffff.
static inline std::uint32_t rgb_low_swapped(std::uint32_t argb) {
    const std::uint32_t r = (argb >> 16) & 0xffu;
    const std::uint32_t g = (argb >>  8) & 0xffu;
    const std::uint32_t b =  argb        & 0xffu;
    return (b << 16) | (g << 8) | r;
}

}  // namespace

// ---------------------------------------------------------------------------
// GradientQuadHorizAlpha  --  0x00473540
//
// void(float x, float y, float w, float h, uint argb)
//
// Screen-scaled quad (same scaling block as ChromeBaseDraw). Per-vertex color
// is a horizontal alpha gradient built directly above the Z/W loop:
//   DAT_00898a30 (V0) = (A<<24) | rgb_low      // A = (argb>>24)&0xff
//   DAT_00898a4c (V1) =          rgb_low       // alpha byte forced to 0
//   _DAT_00898a68 (V2) = DAT_00898a30 (= V0)
//   _DAT_00898a84 (V3) = DAT_00898a4c (= V1)
// rgb_low = (B<<16)|(G<<8)|R  (cited at 0x00473540 body).
// Render states 1=0, 12=1, 10=5, 11=6 then DrawPrimitive(4, &buf, 4).
// ---------------------------------------------------------------------------

// 0x00473540
extern "C" __declspec(dllexport) void __cdecl GradientQuadHorizAlpha(
    float x, float y, float w, float h, std::uint32_t argb)
{
    // Coordinate scaling — cited at 0x00473540 body.
    const float scale_x = *reinterpret_cast<float*>(kScaleX);
    const float scale_y = *reinterpret_cast<float*>(kScaleY);
    const float sx = static_cast<float>(s_ScreenWidthGet())  * x * scale_x;
    const float sy = static_cast<float>(s_ScreenHeightGet()) * y * scale_y;
    const float sw = static_cast<float>(s_ScreenWidthGet())  * w * scale_x;
    const float sh = static_cast<float>(s_ScreenHeightGet()) * h * scale_y;

    fill_xy_quad(sx, sy, sw, sh);

    // Per-vertex colors — cited at 0x00473540 body.
    const std::uint32_t rgb   = rgb_low_swapped(argb);
    const std::uint32_t a     = (argb >> 24) & 0xffu;
    const std::uint32_t colV0 = (a << 24) | rgb;  // V0 / V2 keep alpha
    const std::uint32_t colV1 = rgb;               // V1 / V3 alpha = 0
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x10) = colV0;
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x10) = colV1;
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x10) = colV0;
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x10) = colV1;

    fill_zw_all(rw_z_field());

    rw_set_state(1, 0);
    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

RH_ScopedInstall(GradientQuadHorizAlpha, 0x00473540);

// ---------------------------------------------------------------------------
// BorderQuadFourAlpha  --  0x004736c0
//
// void(float x, float y, float w, float h, uint argb,
//      byte aV0, byte aV1, byte aV2, byte aV3)
//
// Screen-scaled quad (same scaling block). Four independent per-vertex alpha
// bytes (param_6..param_9) are each combined with the shared swapped RGB of
// param_5:
//   DAT_00898a30 (V0) = (aV0<<24) | rgb_low
//   DAT_00898a4c (V1) = (aV1<<24) | rgb_low
//   _DAT_00898a68 (V2) = (aV2<<24) | rgb_low
//   _DAT_00898a84 (V3) = (aV3<<24) | rgb_low
// rgb_low = (B<<16)|(G<<8)|R, with R = (argb>>16)&0xff initially staged in the
// V3-color slot (cited at 0x004736c0 body). Render states + draw identical to
// the gradient sibling.
// ---------------------------------------------------------------------------

// 0x004736c0
extern "C" __declspec(dllexport) void __cdecl BorderQuadFourAlpha(
    float x, float y, float w, float h, std::uint32_t argb,
    std::uint8_t aV0, std::uint8_t aV1, std::uint8_t aV2, std::uint8_t aV3)
{
    const float scale_x = *reinterpret_cast<float*>(kScaleX);
    const float scale_y = *reinterpret_cast<float*>(kScaleY);
    const float sx = static_cast<float>(s_ScreenWidthGet())  * x * scale_x;
    const float sy = static_cast<float>(s_ScreenHeightGet()) * y * scale_y;
    const float sw = static_cast<float>(s_ScreenWidthGet())  * w * scale_x;
    const float sh = static_cast<float>(s_ScreenHeightGet()) * h * scale_y;

    fill_xy_quad(sx, sy, sw, sh);

    // Per-vertex colors — cited at 0x004736c0 body.
    const std::uint32_t rgb = rgb_low_swapped(argb);
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x10) = (static_cast<std::uint32_t>(aV0) << 24) | rgb;
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x10) = (static_cast<std::uint32_t>(aV1) << 24) | rgb;
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x10) = (static_cast<std::uint32_t>(aV2) << 24) | rgb;
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x10) = (static_cast<std::uint32_t>(aV3) << 24) | rgb;

    fill_zw_all(rw_z_field());

    rw_set_state(1, 0);
    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

RH_ScopedInstall(BorderQuadFourAlpha, 0x004736c0);
