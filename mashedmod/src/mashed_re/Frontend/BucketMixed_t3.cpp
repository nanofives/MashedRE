// Mashed RE - Frontend bucket-0041dc30 mixed-cluster C2->C3 session
// c3-batch-t-s3 (2026-05-26).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// 2 promotions + 2 staged in this file:
//   0x00431d90  FrontendPanelFlagAdvance  -- void(void); 19 panel-flag writes       [C3]
//   0x00432ad0  MenuDimOverlayFadeStep    -- void(void); alpha-counter fade-out tick [C3]
//   0x00431b70  GetDat007f0f10            -- pure-leaf uint32(void) global getter   [C2-staged,
//                                            C3 caller-gate fails: sole caller C1]
//   0x00472dc0  Im2DTriangleDraw          -- void(6f,uint32); 3-vertex Im2D fill     [C2-staged,
//                                            live-Z field defeats synthetic diff; hook disabled]
//
// Analysis notes:
//   re/analysis/bucket_0041dc30/0x00431b70.md
//   re/analysis/bucket_0041dc30/0x00432ad0.md
//   re/analysis/options_menu/0x00431d90.md
//   re/analysis/hud_frontend/0x00472dc0.md
//   re/analysis/frontend_c1_to_c2_followup_s2/FUN_00432ad0.md
//   re/analysis/frontend_c1_to_c2_followup_s3/FUN_00472dc0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x00431b70  GetDat007f0f10
//
// Original: FUN_00431b70 (6 bytes, 0x00431b70..0x00431b75)
// Body: `MOV EAX, [0x007f0f10]; RET`  (single-instruction getter leaf).
// Returns: DAT_007f0f10 as uint32. No callees, no writes.
// ref: re/analysis/bucket_0041dc30/0x00431b70.md
//      re/analysis/frontend_c1_to_c2_followup_s1/FUN_00431b70.md
// ---------------------------------------------------------------------------

// 0x00431b70
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat007f0f10() {
    return *reinterpret_cast<std::uint32_t*>(0x007f0f10u);
}

// RH_ScopedInstall(GetDat007f0f10, 0x00431b70);
// STAGED at C2 (NOT promoted by c3-batch-t-s3 2026-05-26).
// The reimpl is bit-identical (read_global Frida diff 10/10 GREEN) and the
// decomp is a trivial getter leaf, BUT the C3 caller-AND-callee gate fails:
// the sole caller FUN_0045d0e0 is C1 (confirmed via Ghidra function_callers),
// and a leaf has no callee. Re-pickup: promote FUN_0045d0e0 to C2+.


// ---------------------------------------------------------------------------
// 0x00431d90  FrontendPanelFlagAdvance
//
// Original: FUN_00431d90 (403 bytes, 0x00431d90..0x00431f23)
// Signature: void __cdecl FUN_00431d90(void)
//
// Body (mechanically): writes 19 frontend panel-flag globals using the
// expression  X = (X != 1) - 1 & 2.  This evaluates as:
//   X == 0 -> 0
//   X == 1 -> 2 (advance to fade-out state)
//   X != 0,1 -> 0 (clear to off)
//
// Exception (cited at 0x00431d90 special-case branch):
//   DAT_0067e7b0 uses `if (DAT_0067e7b0 != 1) DAT_0067e7b0 = 0;`
//   (preserves value 1 instead of converting to 2).
//
// 19 globals enumerated from re/analysis/options_menu/0x00431d90.md:
//   0x0067e7d8 0x0067e7b0(special) 0x0067e7f0 0x0067e7d0 0x0067e7a8
//   0x0067e7c0 0x0067e7c8 0x0067e7f8 0x0067e838 0x0067e828 0x0067e830
//   0x0067e820 0x0067e7e0 0x0067e7e8 0x0067e808 0x0067e810 0x0067e818
//   0x0067e7b8 0x0067e800
// ---------------------------------------------------------------------------

namespace {

inline void advance_flag(std::uintptr_t addr) {
    // (X != 1) - 1 & 2: X==1 -> 2; else -> 0.
    auto p = reinterpret_cast<std::uint32_t*>(addr);
    *p = ((*p != 1u) - 1u) & 2u;
}

inline void advance_flag_options(std::uintptr_t addr) {
    // Special case: preserve 1 if already 1; else clear to 0.
    auto p = reinterpret_cast<std::uint32_t*>(addr);
    if (*p != 1u) {
        *p = 0u;
    }
}

}  // namespace

// 0x00431d90
extern "C" __declspec(dllexport) void __cdecl FrontendPanelFlagAdvance() {
    advance_flag(0x0067e7d8u);
    advance_flag_options(0x0067e7b0u);  // options panel: special preserve-1
    advance_flag(0x0067e7f0u);
    advance_flag(0x0067e7d0u);
    advance_flag(0x0067e7a8u);
    advance_flag(0x0067e7c0u);
    advance_flag(0x0067e7c8u);
    advance_flag(0x0067e7f8u);
    advance_flag(0x0067e838u);
    advance_flag(0x0067e828u);
    advance_flag(0x0067e830u);
    advance_flag(0x0067e820u);
    advance_flag(0x0067e7e0u);
    advance_flag(0x0067e7e8u);
    advance_flag(0x0067e808u);
    advance_flag(0x0067e810u);
    advance_flag(0x0067e818u);
    advance_flag(0x0067e7b8u);
    advance_flag(0x0067e800u);
}

RH_ScopedInstall(FrontendPanelFlagAdvance, 0x00431d90);


// ---------------------------------------------------------------------------
// 0x00432ad0  MenuDimOverlayFadeStep
//
// Original: FUN_00432ad0 (92 bytes, 0x00432ad0..0x00432b2b)
// Signature: void __cdecl FUN_00432ad0(void)
//
// Body (cited from re/analysis/bucket_0041dc30/0x00432ad0.md):
//   - Reads DAT_00898a90 (int alpha counter, expected range 0..0x1ff).
//   - If < 1: DAT_00898a90 = 0; return.
//   - If > 0x1ff: clamp to 0x1ff.
//   - uVar1 = DAT_0067eca8 (save).
//   - DAT_0067eca8 = min(DAT_00898a90, 0xff).
//   - call FUN_0042aae0(0) [MenuIm2DQuad, C3].
//   - DAT_0067eca8 = uVar1 (restore).
//   - DAT_00898a90 -= 0x10 (16, fade-step).
//
// Constants (cited at body):
//   0x1ff (511)  alpha counter upper bound
//   0xff  (255)  overlay alpha cap
//   0x10  (16)   per-frame decrement
//
// Callee FUN_0042aae0 (MenuIm2DQuad) is C3-impl in MenuChrome.cpp; calling
// through its raw VA is safe whether hooked or not.
// ---------------------------------------------------------------------------

using Im2DQuad_t = void(__cdecl*)(int);
static auto* const s_MenuIm2DQuad = reinterpret_cast<Im2DQuad_t>(0x0042aae0);

// 0x00432ad0
extern "C" __declspec(dllexport) void __cdecl MenuDimOverlayFadeStep() {
    auto* pCounter = reinterpret_cast<std::int32_t*>(0x00898a90u);
    auto* pAlpha   = reinterpret_cast<std::int32_t*>(0x0067eca8u);

    std::int32_t v = *pCounter;
    if (v < 1) {
        *pCounter = 0;
        return;
    }
    if (v > 0x1ff) {
        v = 0x1ff;
        *pCounter = v;
    }

    std::int32_t saved = *pAlpha;
    std::int32_t capped = (v > 0xff) ? 0xff : v;
    *pAlpha = capped;
    s_MenuIm2DQuad(0);
    *pAlpha = saved;
    *pCounter = v - 0x10;
}

RH_ScopedInstall(MenuDimOverlayFadeStep, 0x00432ad0);


// ---------------------------------------------------------------------------
// 0x00472dc0  Im2DTriangleDraw
//
// Original: FUN_00472dc0 (378 bytes, 0x00472dc0..0x00472f3a)
// Signature: void __cdecl FUN_00472dc0(float x1, float y1, float x2, float y2,
//                                       float x3, float y3, uint32 argb)
//
// Body (cited from re/analysis/hud_frontend/0x00472dc0.md):
//   Scales each coord pair independently:
//     fVar1 = ScreenWidth() * x1 * _DAT_005cd5a8
//     fVar2 = ScreenHeight() * y1 * _DAT_005cc560
//     fVar3 = ScreenWidth() * x2 * _DAT_005cd5a8
//     fVar4 = ScreenHeight() * y2 * _DAT_005cc560
//     fVar5 = ScreenWidth() * x3 * _DAT_005cd5a8
//     _DAT_00898a5c = ScreenHeight() * y3 * _DAT_005cc560
//   Writes 3-vertex buffer at 0x00898a20:
//     V0 (0x898a20): X=fVar1, Y=fVar2
//     V1 (0x898a3c): X=fVar3, Y=fVar4
//     V2 (0x898a58): X=fVar5, Y=DAT_00898a5c
//     All vertices: Z = *(DAT_007d3ff8 + 0x18), W = 1.0f, color = R<->B swap of argb.
//   Loop runs until puVar8 < 0x898a7c — 3 vertices only.
//   Render states via vtable +0x20:
//     state 1  = 0
//     state 12 = 1  (alpha-blend on)
//     state 10 = 5
//     state 11 = 6
//   Draws via vtable +0x30: fn(4, &DAT_00898a20, 3) — primitive type 4, 3 verts.
//
// Constants (cited at body):
//   0x005cd5a8  X screen-scale multiplier
//   0x005cc560  Y screen-scale multiplier
//   0x00898a20  RwIm2D vertex buffer base
//   0x00898a7c  loop upper bound = end of 3-vertex range
//   0x3f800000  W = 1.0f
//   primitive type 4, vertex count 3
//
// Structural sibling of ChromeBaseDraw (0x00472c60), which is C3 in
// DrawQuadPrimitives.cpp; this is the 3-vertex (triangle) variant.
// ---------------------------------------------------------------------------

namespace {

constexpr std::uintptr_t kIm2DVBufBase = 0x00898a20u;
constexpr std::uintptr_t kIm2DVtable   = 0x007d3ff8u;
constexpr std::size_t    kIm2DVStride  = 0x1cu;       // 28 bytes per vertex
constexpr std::uintptr_t kScaleX       = 0x005cd5a8u;
constexpr std::uintptr_t kScaleY       = 0x005cc560u;

using TriScreenW_t  = std::uint16_t(__cdecl*)();
using TriScreenH_t  = std::uint16_t(__cdecl*)();
using TriSetState_t = void(__cdecl*)(int, int);
using TriDrawPrim_t = void(__cdecl*)(int, void*, int);

inline std::uint16_t tri_screen_width() {
    return reinterpret_cast<TriScreenW_t>(0x0042b8b0)();
}
inline std::uint16_t tri_screen_height() {
    return reinterpret_cast<TriScreenH_t>(0x0042b8c0)();
}
inline std::uintptr_t tri_vtable_base() {
    return *reinterpret_cast<std::uintptr_t*>(kIm2DVtable);
}
inline void tri_set_state(int a, int b) {
    auto fn = *reinterpret_cast<TriSetState_t*>(tri_vtable_base() + 0x20);
    fn(a, b);
}
inline std::uint32_t tri_z_field() {
    return *reinterpret_cast<std::uint32_t*>(tri_vtable_base() + 0x18);
}
inline void tri_draw_3verts() {
    auto fn = *reinterpret_cast<TriDrawPrim_t*>(tri_vtable_base() + 0x30);
    fn(4, reinterpret_cast<void*>(kIm2DVBufBase), 3);
}

// ARGB -> ABGR byte swap: swap R and B bytes, keep A and G.
// Confirmed pattern from sibling ChromeBaseDraw (0x00472c60) in
// DrawQuadPrimitives.cpp; verified bit-identical via draw_quad_observe.
inline std::uint32_t tri_color_swap(std::uint32_t argb) {
    return (argb & 0xff00ff00u)
         | ((argb & 0x00ff0000u) >> 16)
         | ((argb & 0x000000ffu) << 16);
}

}  // namespace

// 0x00472dc0
extern "C" __declspec(dllexport) void __cdecl Im2DTriangleDraw(
        float x1, float y1, float x2, float y2,
        float x3, float y3, std::uint32_t argb) {

    // Coord scaling — cited at 0x00472dc0 body. Each coord re-fetches the
    // screen-dimension getter (matches the original's 6 interleaved calls and
    // the verified sibling ChromeBaseDraw at 0x00472c60); the getters return a
    // stable global so the order is bit-irrelevant.
    const float kx = *reinterpret_cast<float*>(kScaleX);
    const float ky = *reinterpret_cast<float*>(kScaleY);

    const float fVar1 = static_cast<float>(static_cast<int>(tri_screen_width()))  * x1 * kx;
    const float fVar2 = static_cast<float>(static_cast<int>(tri_screen_height())) * y1 * ky;
    const float fVar3 = static_cast<float>(static_cast<int>(tri_screen_width()))  * x2 * kx;
    const float fVar4 = static_cast<float>(static_cast<int>(tri_screen_height())) * y2 * ky;
    const float fVar5 = static_cast<float>(static_cast<int>(tri_screen_width()))  * x3 * kx;
    const float fVar6 = static_cast<float>(static_cast<int>(tri_screen_height())) * y3 * ky;

    // Write XY for each of 3 vertices.
    auto vfloat = [](std::uintptr_t a, float v) {
        *reinterpret_cast<float*>(a) = v;
    };

    vfloat(kIm2DVBufBase + 0 * kIm2DVStride + 0x00, fVar1);  // V0 X
    vfloat(kIm2DVBufBase + 0 * kIm2DVStride + 0x04, fVar2);  // V0 Y
    vfloat(kIm2DVBufBase + 1 * kIm2DVStride + 0x00, fVar3);  // V1 X
    vfloat(kIm2DVBufBase + 1 * kIm2DVStride + 0x04, fVar4);  // V1 Y
    vfloat(kIm2DVBufBase + 2 * kIm2DVStride + 0x00, fVar5);  // V2 X
    vfloat(kIm2DVBufBase + 2 * kIm2DVStride + 0x04, fVar6);  // V2 Y (= DAT_00898a5c)

    // Loop writing Z, W, color for each of 3 vertices (per orig: stride 7 dwords).
    const std::uint32_t z = tri_z_field();
    const std::uint32_t c = tri_color_swap(argb);
    for (int i = 0; i < 3; i++) {
        std::uintptr_t v = kIm2DVBufBase + i * kIm2DVStride;
        *reinterpret_cast<std::uint32_t*>(v + 0x08) = z;             // Z
        *reinterpret_cast<std::uint32_t*>(v + 0x0c) = 0x3f800000u;   // W = 1.0f
        *reinterpret_cast<std::uint32_t*>(v + 0x10) = c;             // color
    }

    // Render states.
    tri_set_state(1, 0);
    tri_set_state(12, 1);
    tri_set_state(10, 5);
    tri_set_state(11, 6);

    // Draw 3-vert primitive.
    tri_draw_3verts();
}

// RH_ScopedInstall(Im2DTriangleDraw, 0x00472dc0);
// STAGED at C2-impl (NOT promoted by c3-batch-t-s3 2026-05-26).
// The reimpl is bit-identical to the original decomp (verified field-by-field
// against FUN_00472dc0 / FUN_00472c60 in Ghidra: coord scale, color bit-math,
// Z/W, render states, 3-vertex draw all match). HOWEVER the draw_quad_observe
// Frida diff is non-deterministic: each vertex's Z field is the LIVE value
// *(DAT_007d3ff8+0x18), read at call time by BOTH orig and reimpl. When a
// frame boundary falls between the orig call and the reimpl call, the Z field
// changes and the buffer fingerprint diverges (observed deterministically on
// the 2 vectors whose color is swap-variant AND a frame advanced — idx1/idx3).
// idx3 proves it: its ARGB 0xff00ff00 is byte-swap-invariant, so the color
// bytes are identical between sides, isolating the divergence to the shared
// live Z read — not the implementation. crash_equal_ok masks the divergence
// inconsistently (GREEN under parallel runner, RED under run_diff.py), so
// promoting on it would be flaky overclaiming.
// Re-pickup: a draw-quad arg_type variant that freezes *(DAT_007d3ff8+0x18)
// (or saves/restores it) around the orig/reimpl call pair, OR a
// canonical-scenario observation with the hook installed.
