// Mashed RE — Frontend HUD cluster, c3-batch-v session 3.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Covers (C2->C3):
//   0x00428610  ViewportScaledRectDraw — viewport-scaled textured rect; 7 args;
//               wraps HudIm2DQuad (0x00450b10) after coord scaling + blend setup.
//
// Skips:
//   0x00458630  FUN_00458630 — powerup-type->name lookup; callee FUN_004c5c00
//               return value is a linked-list-walk pointer (non-deterministic);
//               arg_type EXPLICITLY REFUSED in hooks_registry.py for FUN_004c5c00.
//               No viable Frida diff path. Skipped.
//   0x00423b00  FUN_00423b00 — frontend gate dispatcher; caller FUN_00425a40
//               is only C1; C3 gate (c) fails. Skipped.
//
// Analysis notes:
//   re/analysis/frontend_hud_dispatcher_ae2/0x00428610.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Shared constants (cited from 0x00428610 body and sibling analysis in
// DrawQuadPrimitives.cpp).
// ---------------------------------------------------------------------------
namespace {

// Scale globals — same constants used by ChromeBaseDraw / sibling draw fns.
// _DAT_005cd5a8: width normalisation factor  ≈ 0.001 (1/1000).  Ghidra 0x005cd5a8.
// _DAT_005cc560: height normalisation factor ≈ 0.000833.         Ghidra 0x005cc560.
constexpr std::uintptr_t kScaleX = 0x005cd5a8u;
constexpr std::uintptr_t kScaleY = 0x005cc560u;

// ScreenWidthGet / ScreenHeightGet — 0-arg, already C3 (hooks.csv).
using ShortGetter_t = std::uint16_t(__cdecl*)();
auto* const s_ScreenWidthGet  = reinterpret_cast<ShortGetter_t>(0x0042b8b0u);
auto* const s_ScreenHeightGet = reinterpret_cast<ShortGetter_t>(0x0042b8c0u);

// HudIm2DQuad — 0x00450b10; C3; the terminal draw call.
// Signature (from DrawQuadPrimitives.cpp + hooks.csv):
//   void(int32 tex_handle, float x, float y, float w, float h,
//        uint32 argb, uint32_t* uv_ptr)
using HudIm2DQuad_t = void(__cdecl*)(
    std::int32_t  tex_handle,
    float         x,
    float         y,
    float         w,
    float         h,
    std::uint32_t argb,
    std::uint32_t* uv_ptr);
auto* const s_HudIm2DQuad = reinterpret_cast<HudIm2DQuad_t>(0x00450b10u);

// float constants used in the colour struct initialisation (cited at body):
//   _DAT_005cc320 = 0x3f800000 = 1.0f  (Ghidra 0x005cc320)
//   DAT_005d757c  = 0x00000000 = 0.0f  (Ghidra 0x005d757c)
// 0.5f multiplier for coordMode==2 (cited at 0x428722 branch):
//   _DAT_005cc32c = 0x3f000000 = 0.5f  (Ghidra 0x005cc32c)
constexpr std::uintptr_t kOne  = 0x005cc320u;  // float 1.0f
constexpr std::uintptr_t kZero = 0x005d757cu;  // float 0.0f
constexpr std::uintptr_t kHalf = 0x005cc32cu;  // float 0.5f

}  // namespace

// ---------------------------------------------------------------------------
// ViewportScaledRectDraw  --  0x00428610
//
// 7-arg viewport-scaled textured rect.  Wraps HudIm2DQuad after computing
// normalised screen coords and a 4-float "blend struct" (used as the UV/color
// param to HudIm2DQuad).
//
// Decompilation from Ghidra (pool2, read-only), cited at 0x00428610 body:
//
//   sVar2  = FUN_0042b8b0()                            -- screen width
//   fVar4  = (float)(int)sVar2 * _DAT_005cd5a8         -- normalised width scale
//   sVar2  = FUN_0042b8c0(fVar4)                       -- screen height (arg ignored)
//   fVar1  = (float)(int)sVar2 * _DAT_005cc560         -- normalised height scale
//
//   -- Colour struct defaults (DAT_005d757c = 0.0f, _DAT_005cc320 = 1.0f):
//   local_10 = DAT_005d757c  (0.0f)
//   local_c  = DAT_005d757c  (0.0f)
//   local_8  = _DAT_005cc320 (1.0f)
//   local_4  = _DAT_005cc320 (1.0f)
//
//   -- Coord position from param_6 (coordMode):
//   if (param_6 == 0 || param_6 == 1):
//     local_20 = fVar4 * param_2
//     local_1c = fVar1 * param_3
//   if (param_6 == 2):
//     local_20 = fVar4*param_2 - fVar4*param_4 * _DAT_005cc32c  (subtract half-width)
//     local_1c = fVar1*param_3 - fVar1*param_5 * _DAT_005cc32c  (subtract half-height)
//
//   -- Blend struct from param_7 (blendMode):
//   if (param_7 == 1): local_10 = 1.0f; local_8 = 0.0f
//   if (param_7 == 2): local_c  = 1.0f; local_4 = 0.0f
//   if (param_7 == 3): local_10 = 1.0f; local_8 = 0.0f; local_c = 1.0f; local_4 = 0.0f
//   else: goto LAB_00428722 (no change to struct)
//
//   -- Texture handle:
//   if (param_1 != NULL): uVar3 = *param_1
//   else:                 uVar3 = 0
//
//   -- Draw call:
//   FUN_00450b10(uVar3, local_20, local_1c, fVar4*param_4, fVar1*param_5,
//                0xffffffff, &local_10)
//
// Callee gate:  FUN_0042b8b0 (ScreenWidthGet C3), FUN_0042b8c0 (ScreenHeightGet C3),
//               FUN_00450b10 (HudIm2DQuad C3) — all C3 — gate (b) satisfied.
// Caller gate:  FUN_00428760 (C2) — gate (c) satisfied.
// Uncertainties in note: U-5593 (ScreenHeightGet purpose), U-5594 (blend struct
// semantics) — neither marked [Blocks C3].
// ---------------------------------------------------------------------------

// 0x00428610
extern "C" __declspec(dllexport) void __cdecl ViewportScaledRectDraw(
    std::uint32_t* param_1,   // pointer to texture handle (or NULL)
    float          param_2,   // x position (normalised)
    float          param_3,   // y position (normalised)
    float          param_4,   // width (normalised)
    float          param_5,   // height (normalised)
    int            param_6,   // coordMode: 0/1 = absolute, 2 = centred
    int            param_7)   // blendMode: 0=default, 1=blend_r, 2=blend_g, 3=both
{
    // ── Step 1: compute viewport-normalised scale factors. ───────────────────
    // Cited at 0x00428610 body.
    const float scale_x = *reinterpret_cast<float*>(kScaleX);  // ≈ 0.001
    const float scale_y = *reinterpret_cast<float*>(kScaleY);  // ≈ 0.000833
    const float fVar4   = static_cast<float>(static_cast<int>(s_ScreenWidthGet()))  * scale_x;
    const float fVar1   = static_cast<float>(static_cast<int>(s_ScreenHeightGet())) * scale_y;

    // ── Step 2: colour struct defaults (cited at 0x00428610 body). ───────────
    // Four floats on stack; order as cited in analysis note.
    float local_10 = *reinterpret_cast<float*>(kZero);  // 0.0f
    float local_c  = *reinterpret_cast<float*>(kZero);  // 0.0f
    float local_8  = *reinterpret_cast<float*>(kOne);   // 1.0f
    float local_4  = *reinterpret_cast<float*>(kOne);   // 1.0f

    // ── Step 3: position from coordMode (cited at branch 0x00428683). ────────
    float local_20, local_1c;
    if (param_6 == 0 || param_6 == 1) {
        local_20 = fVar4 * param_2;
        local_1c = fVar1 * param_3;
    } else if (param_6 == 2) {
        const float half = *reinterpret_cast<float*>(kHalf);  // 0.5f
        local_20 = fVar4 * param_2 - fVar4 * param_4 * half;
        local_1c = fVar1 * param_3 - fVar1 * param_5 * half;
    } else {
        // param_6 not 0, 1, or 2: Ghidra shows fall-through to same as 0/1.
        local_20 = fVar4 * param_2;
        local_1c = fVar1 * param_3;
    }

    // ── Step 4: blend struct from blendMode (cited at 0x004286b8 branch). ────
    if (param_7 == 1) {
        local_10 = *reinterpret_cast<float*>(kOne);   // 1.0f
        local_8  = *reinterpret_cast<float*>(kZero);  // 0.0f
    } else if (param_7 == 2) {
        local_c  = *reinterpret_cast<float*>(kOne);   // 1.0f
        local_4  = *reinterpret_cast<float*>(kZero);  // 0.0f
    } else if (param_7 == 3) {
        local_10 = *reinterpret_cast<float*>(kOne);   // 1.0f
        local_8  = *reinterpret_cast<float*>(kZero);  // 0.0f
        local_c  = *reinterpret_cast<float*>(kOne);   // 1.0f
        local_4  = *reinterpret_cast<float*>(kZero);  // 0.0f
    }
    // default (blendMode not 1/2/3): goto LAB_00428722 — no change; fall through.

    // ── Step 5: texture handle (cited at 0x00428728 branch). ─────────────────
    std::uint32_t uVar3;
    if (param_1 != nullptr) {
        uVar3 = *param_1;
    } else {
        uVar3 = 0u;
    }

    // ── Step 6: draw call (cited at 0x00428736). ─────────────────────────────
    // Args: (tex_handle, x, y, w, h, argb=0xffffffff, &blend_struct)
    // The blend struct {local_10, local_c, local_8, local_4} is passed as the
    // uv_ptr argument to HudIm2DQuad.
    std::uint32_t blend_struct[4] = {
        *reinterpret_cast<std::uint32_t*>(&local_10),
        *reinterpret_cast<std::uint32_t*>(&local_c),
        *reinterpret_cast<std::uint32_t*>(&local_8),
        *reinterpret_cast<std::uint32_t*>(&local_4),
    };
    s_HudIm2DQuad(
        static_cast<std::int32_t>(uVar3),
        local_20,
        local_1c,
        fVar4 * param_4,
        fVar1 * param_5,
        0xffffffffu,
        blend_struct);
}

RH_ScopedInstall(ViewportScaledRectDraw, 0x00428610);
