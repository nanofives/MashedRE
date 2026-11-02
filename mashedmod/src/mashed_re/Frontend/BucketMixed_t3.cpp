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
//   0x00431b70  (GetDat007f0f10 deleted, dualcopy4: MenuLeaves_af5.cpp owns this RVA)
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
// 0x00431b70  (dualcopy4, 2026-10-16: the GetDat007f0f10 body that lived here was a
// SECOND, never-installed copy of Frontend/MenuLeaves_af5.cpp:MenuFlagDat007f0f10Get,
// which owns the install and the live registry key. Deleted; its STAGED registry key
// get_dat_007f0f10 is retired. Original: FUN_00431b70, `MOV EAX, [0x007f0f10]; RET`.)
// ref: re/analysis/bucket_0041dc30/0x00431b70.md
//      re/analysis/frontend_c1_to_c2_followup_s1/FUN_00431b70.md
// ---------------------------------------------------------------------------


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

// MATCH-S07: the body of 0x00431d90 (FrontendPanelFlagAdvance) moved to Match/MatchS07_00431d90.cpp,
// which compiles the byte-matched source re/match/s07/00431d90.c.


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

// MATCH-S08: the body of 0x00432ad0 (MenuDimOverlayFadeStep) moved to Match/MatchS08_00432ad0.cpp,
// which compiles the byte-matched source re/match/s08/00432ad0.c.


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

// MATCH-S11: the body of 0x00472dc0 (Im2DTriangleDraw) moved to Match/MatchS11_00472dc0.cpp,
// which compiles the byte-matched source re/match/s11/00472dc0.c.

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
