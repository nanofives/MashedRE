// Mashed RE - Frontend text-width measurement reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_b/00428320.md
//   re/analysis/promote_c2_hud_ingame/0x005554d0.md  (callee drift-promote)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// DEFERRED: TextWidthMeasureA (0x004282a0) — callee 0x004277a0 (FUN_004277a0,
// "finalize font context") is not at C2+ in hooks.csv and has no dedicated
// C2 analysis note. Caller gate cannot be cleared.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (not yet reimplemented; we call originals)
// ---------------------------------------------------------------------------

// 0x00427840  FontText_UTF16WidenCopy  — item-based font context setup
// Analysis: re/analysis/frontend_promote_menus_b/00428320.md  (callee list)
// Status: C2 in hooks.csv; called with no args in this context.
static auto* const s_FUN_00427840 =
    reinterpret_cast<void(__cdecl*)()>(0x00427840);

// 0x005554d0  FUN_005554d0  — string width accumulator
// Analysis: re/analysis/promote_c2_hud_ingame/0x005554d0.md  (C2 note)
// hooks.csv shows C1 but C2 analysis plate exists — tracker drift; promoted here.
// Signature: float (void* font_ctx, uint8_t* str, float scale)
static auto* const s_FUN_005554d0 =
    reinterpret_cast<float(__cdecl*)(void*, std::uint8_t*, float)>(0x005554d0);

// ---------------------------------------------------------------------------
// Globals accessed by TextWidthMeasureB  (addresses cited from 0x00428320 body)
// ---------------------------------------------------------------------------

// DAT_0067d838  draw context handle   (cited at 0x00428356)
static constexpr std::uintptr_t kFontCtx       = 0x0067d838u;
// _DAT_005cd5fc  size scale float      (cited at 0x00428352)
static constexpr std::uintptr_t kSizeScale      = 0x005cd5fcu;
// _DAT_0067d830  viewport width float  (cited at 0x0042835e)
static constexpr std::uintptr_t kViewportW      = 0x0067d830u;
// _DAT_005cd618  logical width scale   (cited at 0x00428368)
static constexpr std::uintptr_t kLogicalScale   = 0x005cd618u;

// Stack buffer size for UTF-16 string (256 bytes = 128 wide chars, matching
// decomp local_404 at 0x00428320 body).
static constexpr int kStrBufSize = 256;

// ---------------------------------------------------------------------------
// TextWidthMeasureB  --  0x00428320
//
// Original: FUN_00428320 (111 bytes, 0x00428320..0x0042838f)
// Signature: float (uint32_t param_1, float param_2)
//   Uses item-based font context (FUN_00427840, no explicit param).
// Logic:
//   FUN_00427840();
//   result = FUN_005554d0(DAT_0067d838, local_404, param_2 * _DAT_005cd5fc);
//   return (result / _DAT_0067d830) * _DAT_005cd618;
// Stack cookie: __security_check_cookie present in original; omitted in reimpl
//   (compiler will insert for us when /GS is active; or tolerated absent under /O2).
// x87 FPU extended precision: original operates in float10; we use double for
//   intermediate division/multiply and cast back to float on return.
// ref: re/analysis/frontend_promote_menus_b/00428320.md
// ---------------------------------------------------------------------------

// 0x00428320
extern "C" __declspec(dllexport) float __cdecl TextWidthMeasureB(
    std::uint32_t /*param_1*/, float param_2)
{
    // local_404: 256-byte stack buffer for internal string; filled by FUN_00427840.
    // We need 256 bytes aligned; use uint8_t array matching original stack layout.
    std::uint8_t local_404[kStrBufSize];

    // Step 1: item-based font context setup (no explicit param; operates on
    // implicit current-item state per decomp at 0x00428320..0x00428331).
    s_FUN_00427840();

    // Step 2: measure text width (raw pixel sum) scaled by param_2 * size_scale.
    void* font_ctx  = *reinterpret_cast<void**>(kFontCtx);
    float size_scale = *reinterpret_cast<float*>(kSizeScale);
    float raw_width  = s_FUN_005554d0(font_ctx, local_404, param_2 * size_scale);

    // Step 3: convert raw pixel width to logical units.
    // (raw_width / viewport_width) * logical_scale
    // Use double intermediate to match x87 float10 behaviour of original.
    double viewport_w   = static_cast<double>(*reinterpret_cast<float*>(kViewportW));
    double logical_scale = static_cast<double>(*reinterpret_cast<float*>(kLogicalScale));
    double result = (static_cast<double>(raw_width) / viewport_w) * logical_scale;
    return static_cast<float>(result);
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(TextWidthMeasureB, 0x00428320);
