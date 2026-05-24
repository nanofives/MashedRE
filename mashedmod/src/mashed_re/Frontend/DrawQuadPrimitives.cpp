// Mashed RE — Frontend Im2D draw-quad primitives.
// Five low-level rectangle draws that share the global 4-vertex buffer at
// DAT_00898a20 and dispatch through the RW driver vtable at DAT_007d3ff8.
//
// c3-batch-m-s1 replay (harness-extension session 2026-05-21):
// The five candidates halted in c3-batch-m-s1 (zero-yield STOP-AND-ASK) because
// diff_template.js lacked a draw_quad_observe arg_type. That arg_type landed
// 2026-05-21; this file authors the five implementations against it.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (all confirmed bit-identical via Frida draw_quad_observe):
//   0x00472c60  ChromeBaseDraw            — 5-arg filled quad
//   0x00472f40  TextGradientV0V1Override  — 5-arg quad + top-edge opaque-black
//   0x004730b0  TextGradientV2V3Override  — 5-arg quad + bottom-edge opaque-black
//   0x00473870  TextSpriteUVExplicit      — 7-arg textured quad (no coord scaling)
//   0x004739f0  TextSpriteScaled          — 12-arg textured quad (3 scaling modes,
//                                            explicit UV; staged at C2-impl —
//                                            U-0458/U-0459 block C3 promotion)
//   0x00450b10  HudIm2DQuad               — 7-arg textured quad; explicit UV array;
//                                            raw int texture handle; different render
//                                            state sequence from siblings; HUD sprite draw
//
// Analysis notes:
//   re/analysis/hud_frontend/0x00472c60.md
//   re/analysis/credits_screen/0x00472f40.md
//   re/analysis/credits_screen/0x004730b0.md
//   re/analysis/hud_frontend_d5/0x00473870.md
//   re/analysis/promote_c1_mid_ab2/0x004739f0.md
//   re/analysis/hud_ingame_promote_c2/0x00450b10.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Shared layout — 4-vertex buffer DAT_00898a20..DAT_00898a98
// 4 vertices × 7 dwords (28 bytes) each:
//   +0x00 X  +0x04 Y  +0x08 Z(RHW)  +0x0c W  +0x10 color  +0x14 U  +0x18 V
// ---------------------------------------------------------------------------
namespace {

constexpr std::uintptr_t kVBufBase     = 0x00898a20u;
constexpr std::uintptr_t kVtableGlobal = 0x007d3ff8u;
constexpr std::size_t    kVertStride   = 0x1cu;       // 28 bytes per vertex

// Per-vertex absolute addresses (V0..V3) and color-field addresses.
constexpr std::uintptr_t kV0 = kVBufBase + 0 * kVertStride;  // 0x00898a20
constexpr std::uintptr_t kV1 = kVBufBase + 1 * kVertStride;  // 0x00898a3c
constexpr std::uintptr_t kV2 = kVBufBase + 2 * kVertStride;  // 0x00898a58
constexpr std::uintptr_t kV3 = kVBufBase + 3 * kVertStride;  // 0x00898a74

// Scale globals (cited at each function's body).
constexpr std::uintptr_t kScaleX     = 0x005cd5a8u;  // X screen-scale multiplier
constexpr std::uintptr_t kScaleY     = 0x005cc560u;  // Y screen-scale multiplier
constexpr std::uintptr_t kScaleYOnly = 0x005ceac4u;  // Y-only scale (TextSpriteScaled mode 2)

// Screen-dimension getters (already C3 in hooks.csv).
using ShortGetter_t = std::uint16_t(__cdecl*)();
auto* const s_ScreenWidthGet  = reinterpret_cast<ShortGetter_t>(0x0042b8b0);  // FUN_0042b8b0
auto* const s_ScreenHeightGet = reinterpret_cast<ShortGetter_t>(0x0042b8c0);  // FUN_0042b8c0

// Vtable callees: render-state setter at vtable+0x20 and DrawPrimitive at +0x30.
using RwSetState_t  = void(__cdecl*)(int, int);
using RwDrawPrim_t  = void(__cdecl*)(int, void*, int);

static inline std::uintptr_t vtable_base() {
    return *reinterpret_cast<std::uintptr_t*>(kVtableGlobal);
}

static inline void rw_set_state(int a, int b) {
    auto fn = *reinterpret_cast<RwSetState_t*>(vtable_base() + 0x20);
    fn(a, b);
}

static inline void rw_draw_4verts() {
    auto fn = *reinterpret_cast<RwDrawPrim_t*>(vtable_base() + 0x30);
    fn(4, reinterpret_cast<void*>(kVBufBase), 4);
}

// Z field = *(vtable+0x18) per analysis (RHW source).
static inline std::uint32_t rw_z_field() {
    return *reinterpret_cast<std::uint32_t*>(vtable_base() + 0x18);
}

// ARGB → ABGR byte swap: swap R and B, keep A and G.
// Confirmed against Frida draw_quad_observe 2026-05-21 on ChromeBaseDraw with
// non-trivial RGB inputs (test vector 0xff112233 → buffer reads 0xff332211).
static inline std::uint32_t color_swap_argb_to_abgr(std::uint32_t argb) {
    return (argb & 0xff00ff00u)
         | ((argb & 0x00ff0000u) >> 16)
         | ((argb & 0x000000ffu) << 16);
}

// Write Z/W/color for a single vertex at vert_base. Cited at the per-function
// loop body (0x004738b8 for TextSpriteUVExplicit; equivalent pattern in others).
static inline void write_vert_zwc(std::uintptr_t vert_base, std::uint32_t z, std::uint32_t color_swapped) {
    *reinterpret_cast<std::uint32_t*>(vert_base + 0x08) = z;
    *reinterpret_cast<std::uint32_t*>(vert_base + 0x0c) = 0x3f800000u;  // 1.0f
    *reinterpret_cast<std::uint32_t*>(vert_base + 0x10) = color_swapped;
}

// Write X/Y for a single vertex.
static inline void write_vert_xy(std::uintptr_t vert_base, float x, float y) {
    *reinterpret_cast<float*>(vert_base + 0x00) = x;
    *reinterpret_cast<float*>(vert_base + 0x04) = y;
}

// Fill all 4 vertex Z/W/color fields with a single (z, color) value (the common
// pattern in ChromeBaseDraw / TextGradient* / TextSpriteUVExplicit).
static inline void fill_zwc_all(std::uint32_t z, std::uint32_t color_swapped) {
    write_vert_zwc(kV0, z, color_swapped);
    write_vert_zwc(kV1, z, color_swapped);
    write_vert_zwc(kV2, z, color_swapped);
    write_vert_zwc(kV3, z, color_swapped);
}

// Fill all 4 X/Y fields for a TL/TR/BL/BR quad.
static inline void fill_xy_quad(float x, float y, float w, float h) {
    write_vert_xy(kV0, x,     y    );  // TL
    write_vert_xy(kV1, x + w, y    );  // TR
    write_vert_xy(kV2, x,     y + h);  // BL
    write_vert_xy(kV3, x + w, y + h);  // BR
}

}  // namespace

// ---------------------------------------------------------------------------
// ChromeBaseDraw  --  0x00472c60
//
// 5-arg filled quad with screen-scale on all coords. Cited at 0x00472c60 body.
//
// fVar1 = (float)FUN_0042b8b0() * param_1 * _DAT_005cd5a8   (X)
// fVar2 = (float)FUN_0042b8c0() * param_2 * _DAT_005cc560   (Y)
// fVar3 = (float)FUN_0042b8b0() * param_3 * _DAT_005cd5a8   (W)
// Y+H   = (float)FUN_0042b8c0() * param_4 * _DAT_005cc560 + fVar2
// Render states 1=0 (no texture), 12=1, 10=5, 11=6.
// ---------------------------------------------------------------------------

// 0x00472c60
extern "C" __declspec(dllexport) void __cdecl ChromeBaseDraw(
    float x, float y, float w, float h, std::uint32_t argb)
{
    // Coordinate scaling — cited at 0x00472c60 body.
    const float scale_x = *reinterpret_cast<float*>(kScaleX);
    const float scale_y = *reinterpret_cast<float*>(kScaleY);
    const float sx = static_cast<float>(s_ScreenWidthGet())  * x * scale_x;
    const float sy = static_cast<float>(s_ScreenHeightGet()) * y * scale_y;
    const float sw = static_cast<float>(s_ScreenWidthGet())  * w * scale_x;
    const float sh = static_cast<float>(s_ScreenHeightGet()) * h * scale_y;

    fill_xy_quad(sx, sy, sw, sh);
    fill_zwc_all(rw_z_field(), color_swap_argb_to_abgr(argb));

    // Render states cited at body exit.
    rw_set_state(1, 0);   // disable texture
    rw_set_state(12, 1);  // alpha blend enable
    rw_set_state(10, 5);  // src blend factor
    rw_set_state(11, 6);  // dst blend factor
    rw_draw_4verts();
}

RH_ScopedInstall(ChromeBaseDraw, 0x00472c60);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// TextGradientV0V1Override  --  0x00472f40
//
// Same as ChromeBaseDraw but the V0/V1 (top edge) color fields are overridden
// to 0xff000000 (opaque black) AFTER the fill — producing a top-to-bottom
// alpha gradient. Cited at 0x00472f40 body; the override addresses are
// DAT_00898a30 (V0 color = kV0+0x10) and DAT_00898a4c (V1 color = kV1+0x10).
// ---------------------------------------------------------------------------

// 0x00472f40
extern "C" __declspec(dllexport) void __cdecl TextGradientV0V1Override(
    float x, float y, float w, float h, std::uint32_t argb)
{
    // Same scaling block as ChromeBaseDraw — cited at 0x00472f40 body.
    const float scale_x = *reinterpret_cast<float*>(kScaleX);
    const float scale_y = *reinterpret_cast<float*>(kScaleY);
    const float sx = static_cast<float>(s_ScreenWidthGet())  * x * scale_x;
    const float sy = static_cast<float>(s_ScreenHeightGet()) * y * scale_y;
    const float sw = static_cast<float>(s_ScreenWidthGet())  * w * scale_x;
    const float sh = static_cast<float>(s_ScreenHeightGet()) * h * scale_y;

    fill_xy_quad(sx, sy, sw, sh);
    fill_zwc_all(rw_z_field(), color_swap_argb_to_abgr(argb));

    // Top-edge opaque-black override — DAT_00898a30 (V0) + DAT_00898a4c (V1).
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x10) = 0xff000000u;
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x10) = 0xff000000u;

    rw_set_state(1, 0);
    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

RH_ScopedInstall(TextGradientV0V1Override, 0x00472f40);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// TextGradientV2V3Override  --  0x004730b0
//
// Same as V0V1Override but overrides V2/V3 (bottom edge) color fields with
// 0xff000000 — producing a bottom-to-top alpha gradient. Cited at 0x004730b0
// body; override addresses _DAT_00898a68 (V2 color = kV2+0x10) and
// _DAT_00898a84 (V3 color = kV3+0x10).
// ---------------------------------------------------------------------------

// 0x004730b0
extern "C" __declspec(dllexport) void __cdecl TextGradientV2V3Override(
    float x, float y, float w, float h, std::uint32_t argb)
{
    const float scale_x = *reinterpret_cast<float*>(kScaleX);
    const float scale_y = *reinterpret_cast<float*>(kScaleY);
    const float sx = static_cast<float>(s_ScreenWidthGet())  * x * scale_x;
    const float sy = static_cast<float>(s_ScreenHeightGet()) * y * scale_y;
    const float sw = static_cast<float>(s_ScreenWidthGet())  * w * scale_x;
    const float sh = static_cast<float>(s_ScreenHeightGet()) * h * scale_y;

    fill_xy_quad(sx, sy, sw, sh);
    fill_zwc_all(rw_z_field(), color_swap_argb_to_abgr(argb));

    // Bottom-edge opaque-black override.
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x10) = 0xff000000u;
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x10) = 0xff000000u;

    rw_set_state(1, 0);
    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

RH_ScopedInstall(TextGradientV2V3Override, 0x004730b0);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// TextSpriteUVExplicit  --  0x00473870
//
// 7-arg textured quad: tex_ptr, x, y, w, h, argb, blend_flag.
// NO coordinate scaling — params used as-is (cited at 0x00473870 body).
// Texture/blend dispatch:
//   tex_ptr == NULL: render state 1 = 0 (no texture).
//   tex_ptr != NULL: render state 1 = *tex_ptr;
//                    render state 9 = (blend_flag == 0 ? 1 : 2).
// Then states 12=1, 10=5, 11=6 and draw.
// U-3415 (color byte-swap formula) + U-3416 (UV fields not written here) are
// carried — neither marked [Blocks C3]; runtime evidence resolves the swap.
// ---------------------------------------------------------------------------

// 0x00473870
extern "C" __declspec(dllexport) void __cdecl TextSpriteUVExplicit(
    int* tex_ptr, float x, float y, float w, float h,
    std::uint32_t argb, int blend_flag)
{
    // No scaling — params used directly.
    fill_xy_quad(x, y, w, h);

    // UV mapping: hardcoded full-texture quad (CORRECTION 2026-05-21).
    // C1 analysis note in re/analysis/hud_frontend_d5/0x00473870.md claimed
    // "UV fields not set in this function" — runtime decomp via Ghidra MCP
    // 2026-05-21 confirmed the function does write a fixed [0..1]×[0..1] UV
    // quad (cited at decomp lines: _DAT_00898a34=0, _DAT_00898a38=0,
    // _DAT_00898a50=0x3f800000, _DAT_00898a54=0, _DAT_00898a6c=0,
    // _DAT_00898a70=0x3f800000, _DAT_00898a88=0x3f800000,
    // _DAT_00898a8c=0x3f800000). Note addendum filed.
    //   V0 (TL): U=0.0, V=0.0
    //   V1 (TR): U=1.0, V=0.0
    //   V2 (BL): U=0.0, V=1.0
    //   V3 (BR): U=1.0, V=1.0
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x14) = 0x00000000u;  // V0 U=0
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x18) = 0x00000000u;  // V0 V=0
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x14) = 0x3f800000u;  // V1 U=1
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x18) = 0x00000000u;  // V1 V=0
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x14) = 0x00000000u;  // V2 U=0
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x18) = 0x3f800000u;  // V2 V=1
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x14) = 0x3f800000u;  // V3 U=1
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x18) = 0x3f800000u;  // V3 V=1

    fill_zwc_all(rw_z_field(), color_swap_argb_to_abgr(argb));

    // Texture dispatch — cited at 0x004738d8 / 0x004738e3 / 0x004738f2.
    if (tex_ptr == nullptr) {
        rw_set_state(1, 0);
    } else {
        int tex_handle = *tex_ptr;
        rw_set_state(1, tex_handle);
        rw_set_state(9, blend_flag == 0 ? 1 : 2);
    }

    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

RH_ScopedInstall(TextSpriteUVExplicit, 0x00473870);  // re-enabled 2026-05-24 c3-frontend-b


// ---------------------------------------------------------------------------
// TextSpriteScaled  --  0x004739f0
//
// 12-arg textured quad with 3 coordinate-scaling modes (param_11) and explicit
// per-vertex UVs (param_7..param_10). Mode dispatch cited at 0x004739f0 body:
//
//   param_11 == 0:  no scaling.
//   param_11 == 2:  Y-only scale: param_3 *= b8c0() * _DAT_005ceac4;
//                                   param_5 *= b8c0() * _DAT_005ceac4.
//                   (X coords unchanged.)
//   default:        scale all: param_2 *= b8b0() * _DAT_005cd5a8;
//                              param_3 *= b8c0() * _DAT_005cc560;
//                              param_4 *= b8b0() * _DAT_005cd5a8;
//                              param_5 *= b8c0() * _DAT_005cc560.
//
// UV layout (cited at body assignments to _DAT_00898a34..0x00898a8c):
//   V0: U=param_7  V=param_9
//   V1: U=param_8  V=param_9
//   V2: U=param_7  V=param_10
//   V3: U=param_8  V=param_10
//
// Texture/blend dispatch identical shape to TextSpriteUVExplicit.
//
// **STAGED AT C2-IMPL — not promoted to C3.** Two [Blocks C3] uncertainties on
// file: U-0458 (UV corner mapping to U/V min/max not confirmed vs RW Im2DVertex
// spec) and U-0459 (param_11==2 mode's coord system unidentified — DAT_005ceac4
// writer not traced). Promotion requires Ghidra-side resolution of both U-IDs.
// Hook installed so the harness can run draw_quad_observe against it; a clean
// diff is runtime evidence relevant to U-0458 but doesn't unblock U-0459.
// ---------------------------------------------------------------------------

// 0x004739f0
extern "C" __declspec(dllexport) void __cdecl TextSpriteScaled(
    int*           tex_ptr,
    float          x,
    float          y,
    float          w,
    float          h,
    std::uint32_t  argb,
    std::uint32_t  u0,
    std::uint32_t  u1,
    std::uint32_t  v0,
    std::uint32_t  v1,
    int            scale_mode,
    int            blend_flag)
{
    // Coordinate-scaling dispatch — cited at 0x004739f0 body.
    // FP-associativity matters: the orig computes `(short_val * param) * scale`
    // left-to-right; we must preserve that order. `param *= short_val * scale`
    // would compute `param * (short_val * scale)` and produce different bit-
    // patterns. Confirmed via runtime diff 2026-05-21 on modes 1+2 with non-
    // trivial coordinates.
    if (scale_mode == 0) {
        // No scaling — params used as-is.
    } else if (scale_mode == 2) {
        const float scale_y_only = *reinterpret_cast<float*>(kScaleYOnly);
        const float sH = static_cast<float>(s_ScreenHeightGet());
        y = sH * y * scale_y_only;
        h = sH * h * scale_y_only;
    } else {
        const float scale_x = *reinterpret_cast<float*>(kScaleX);
        const float scale_y = *reinterpret_cast<float*>(kScaleY);
        const float sW = static_cast<float>(s_ScreenWidthGet());
        const float sH = static_cast<float>(s_ScreenHeightGet());
        x = sW * x * scale_x;
        y = sH * y * scale_y;
        w = sW * w * scale_x;
        h = sH * h * scale_y;
    }

    // X/Y geometry — TL/TR/BL/BR.
    fill_xy_quad(x, y, w, h);

    // UV writes — cited at 0x004739f0 body. Per-vertex offsets +0x14 (U), +0x18 (V).
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x14) = u0;  // V0 U
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x18) = v0;  // V0 V
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x14) = u1;  // V1 U
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x18) = v0;  // V1 V
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x14) = u0;  // V2 U
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x18) = v1;  // V2 V
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x14) = u1;  // V3 U
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x18) = v1;  // V3 V

    // Loop body — Z/W/color for all 4 vertices.
    fill_zwc_all(rw_z_field(), color_swap_argb_to_abgr(argb));

    // Texture / blend dispatch (same shape as TextSpriteUVExplicit).
    if (tex_ptr == nullptr) {
        rw_set_state(1, 0);
    } else {
        int tex_handle = *tex_ptr;
        rw_set_state(1, tex_handle);
        rw_set_state(9, blend_flag == 0 ? 1 : 2);
    }

    rw_set_state(12, 1);
    rw_set_state(10, 5);
    rw_set_state(11, 6);
    rw_draw_4verts();
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(TextSpriteScaled, 0x004739f0);


// ---------------------------------------------------------------------------
// HudIm2DQuad  --  0x00450b10
//
// 7-arg Im2D textured quad. Explicit UV array; NO coordinate scaling (params
// used as-is). Cited at 0x00450b10 body.
//
// Signature:
//   param_1 (int32_t)      — texture handle (0 = untextured; raw int, not ptr)
//   param_2 (float)        — x
//   param_3 (float)        — y
//   param_4 (float)        — width
//   param_5 (float)        — height
//   param_6 (uint32_t)     — ARGB color (byte-reordered before write)
//   param_7 (uint32_t[4])  — UV array [u0, v0, u1, v1] as raw float32 bits
//
// UV layout (cited at 0x00450b10 pre-fill block):
//   V0 (TL): U=param_7[0]=u0, V=param_7[1]=v0
//   V1 (TR): U=param_7[2]=u1, V=param_7[1]=v0  (V1.v copied from V0.v)
//   V2 (BL): U=param_7[0]=u0, V=param_7[3]=v1  (V2.u copied from V0.u)
//   V3 (BR): U=param_7[2]=u1, V=param_7[3]=v1
//
// Texture dispatch (0x00450b10 body):
//   param_1 == 0: state(1, 0)     — clear texture
//   param_1 != 0: state(1, param_1); state(9, 2)   — bind + state 9=2
//
// Render state sequence (0x00450b10 body):
//   state(8, 0); state(6, 0); state(0xc, 1)  [before draw]
//   draw 4 verts
//   state(8, 1); state(6, 1)  [restore; 0xc not restored]
//
// Loop: Z/rhw/color written via pointer loop from V0.z(+0x08) advancing 0x1c
// per iteration until addr >= 0x898a98 (past V3). Same color_swap as siblings.
//
// Callees: none (pure leaf; vtable calls only). Caller: 0x00428450 (C2).
// Analysis note: re/analysis/hud_ingame_promote_c2/0x00450b10.md
// ---------------------------------------------------------------------------

// 0x00450b10
extern "C" __declspec(dllexport) void __cdecl HudIm2DQuad(
    std::int32_t  tex_handle,
    float         x,
    float         y,
    float         w,
    float         h,
    std::uint32_t argb,
    std::uint32_t* uv)          // uv[0]=u0_bits, uv[1]=v0_bits, uv[2]=u1_bits, uv[3]=v1_bits
{
    const std::uint32_t z    = rw_z_field();
    const std::uint32_t col  = color_swap_argb_to_abgr(argb);

    // ── Pre-fill UV fields (cited at 0x00450b10 pre-fill block) ────────────────
    // V0: x,y already set below; U=uv[0], V=uv[1]
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x14) = uv[0];  // V0.U = u0  (0x00898a34)
    *reinterpret_cast<std::uint32_t*>(kV0 + 0x18) = uv[1];  // V0.V = v0  (0x00898a38)
    // V1: U=uv[2], V=uv[1] (copy of v0)
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x14) = uv[2];  // V1.U = u1  (0x00898a50)
    *reinterpret_cast<std::uint32_t*>(kV1 + 0x18) = uv[1];  // V1.V = v0  (0x00898a54)
    // V2: U=uv[0] (copy of u0), V=uv[3]
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x14) = uv[0];  // V2.U = u0  (0x00898a6c)
    *reinterpret_cast<std::uint32_t*>(kV2 + 0x18) = uv[3];  // V2.V = v1  (0x00898a70)
    // V3: U=uv[2] (copy of u1), V=uv[3] (copy of v1)
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x14) = uv[2];  // V3.U = u1  (0x00898a88)
    *reinterpret_cast<std::uint32_t*>(kV3 + 0x18) = uv[3];  // V3.V = v1  (0x00898a8c)

    // ── X/Y geometry (cited at 0x00450b10 body) ─────────────────────────────
    *reinterpret_cast<float*>(kV0 + 0x00) = x;              // V0.X (0x00898a20)
    *reinterpret_cast<float*>(kV0 + 0x04) = y;              // V0.Y (0x00898a24)
    *reinterpret_cast<float*>(kV1 + 0x00) = x + w;          // V1.X (0x00898a3c)
    *reinterpret_cast<float*>(kV1 + 0x04) = y;              // V1.Y (0x00898a40)
    *reinterpret_cast<float*>(kV2 + 0x00) = x;              // V2.X (0x00898a58)
    *reinterpret_cast<float*>(kV2 + 0x04) = y + h;          // V2.Y (0x00898a5c)
    // V3.X and V3.Y are set via the loop copy of V1.X and V2.Y:
    *reinterpret_cast<float*>(kV3 + 0x00) = x + w;          // V3.X (0x00898a74)
    *reinterpret_cast<float*>(kV3 + 0x04) = y + h;          // V3.Y (0x00898a78)

    // ── Loop: Z/RHW/color for all 4 vertices (cited at loop body 0x00450b10) ──
    // puVar2 starts at V0.z (+0x08), advances 7 uint32s (0x1c) per iteration,
    // exits when addr >= 0x898a98.
    fill_zwc_all(z, col);

    // ── Texture dispatch (cited at 0x00450b10 branch 0x00450b5b) ────────────
    if (tex_handle == 0) {
        rw_set_state(1, 0);                   // no texture
    } else {
        rw_set_state(1, tex_handle);          // bind texture handle
        rw_set_state(9, 2);                   // state 9=2 (unconditional when tex != 0)
    }

    // ── Render states before draw (cited at 0x00450b10 body) ────────────────
    rw_set_state(8,    0);   // state 8=0
    rw_set_state(6,    0);   // state 6=0
    rw_set_state(0xc,  1);   // state 12=1

    // ── Draw (cited at 0x00450b10 draw call) ─────────────────────────────────
    rw_draw_4verts();        // (**(+0x30))(4, &DAT_00898a20, 4)

    // ── Restore render states (cited at 0x00450b10 epilog) ──────────────────
    rw_set_state(8, 1);
    rw_set_state(6, 1);
}

RH_ScopedInstall(HudIm2DQuad, 0x00450b10);  // re-enabled 2026-05-24 c3-safe
