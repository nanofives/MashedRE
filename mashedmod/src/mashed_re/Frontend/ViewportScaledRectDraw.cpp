// ViewportScaledRectDraw.cpp -- 0x00428610, re-ported from the listing as an x87 TU (bnormalise 2026-10-12).
// Was Frontend/Cluster_v3.cpp (c3-batch-v session 3).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Original: FUN_00428610 (body 0x00428610..0x00428753, `add esp,0x48` at 0x00428750 = 0x2c locals + 7 pushed args).
// Signature: void __cdecl (uint32* tex, float x, float y, float w, float h, int coordMode, int blendMode).
// Read with `py -3.12 re/tools/disasm_fn.py 0x00428610 0x00428754` on original\MASHED.exe.unpatched.
// Callees by Ghidra function boundary: 0x0042b8b0, 0x0042b8c0, 0x00450b10.
//
// DEFECTS IN THE OLD BODY (each corrected from the listing):
//  1. coordMode not 0/1/2: the original leaves x and y at the 0.0 stored at 0x00428615 / 0x0042861d (`jne 0x4286f3`
//     at 0x004286ab skips both stores). The old body fell through to the coordMode-0 formula.
//  2. coordMode 2: 0x004286ad `fld [esp]; fmul x` keeps sx*x in ST and subtracts the STORED w ([esp+0x14], rounded at
//     0x00428695) times [0x5cc32c], rounding once at 0x004286c1 `fstp`. The old SSE2 body rounded sx*x to float first.
//     This TU is in mashedmod/x87_tus.txt: every value the listing keeps in ST is a `double`, rounded to `float` only
//     where the bytes `fstp dword`, so the body follows the caller's precision control as the original does.
// Both were invisible to the old draw_quad_observe entry: its vectors used coordMode 0..2 only, and it read the
// 0x00898a20 vertex block at the harness CW alone. The texture argument never reached that block at all
// (0x00450c15 passes it to the render-state slot, not the vertex buffer).

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

using ShortGetter_t = std::int16_t (__cdecl*)();
using Quad_t        = void (__cdecl*)(std::uint32_t, float, float, float, float, std::uint32_t, float*);

auto* const s_ScreenWidthGet  = reinterpret_cast<ShortGetter_t>(0x0042b8b0u);   // 0x00428635
auto* const s_ScreenHeightGet = reinterpret_cast<ShortGetter_t>(0x0042b8c0u);   // 0x0042864f
auto* const s_HudIm2DQuad     = reinterpret_cast<Quad_t>(0x00450b10u);          // 0x0042874b

constexpr std::uintptr_t kXScale = 0x005cd5a8u;   // 0x00428645 fmul
constexpr std::uintptr_t kYScale = 0x005cc560u;   // 0x00428667 fmul
constexpr std::uintptr_t kZero   = 0x005d757cu;   // 0x00428671 fld
constexpr std::uintptr_t kOne    = 0x005cc320u;   // 0x0042867f fld
constexpr std::uintptr_t kHalf   = 0x005cc32cu;   // 0x004286b9 / 0x004286d1 fmul

inline float  Rf(std::uintptr_t a) { return *reinterpret_cast<const float*>(a); }
inline double Rd(std::uintptr_t a) { return static_cast<double>(Rf(a)); }

}  // namespace

// 0x00428610
extern "C" __declspec(dllexport) void __cdecl ViewportScaledRectDraw(
    std::uint32_t* tex, float x, float y, float w, float h, int coordMode, int blendMode)
{
    // 0x00428615 / 0x0042861d: x and y start at 0.
    float outX = 0.0f;
    float outY = 0.0f;

    // 0x00428635..0x0042864b: sx = (float)((int)(short)ScreenWidthGet() * [0x5cd5a8]) -> [esp].
    const float sx = static_cast<float>(static_cast<double>(static_cast<int>(s_ScreenWidthGet())) * Rd(kXScale));
    // 0x0042864f..0x0042866d: sy = (float)((int)(short)ScreenHeightGet() * [0x5cc560]) -> [esp+4].
    const float sy = static_cast<float>(static_cast<double>(static_cast<int>(s_ScreenHeightGet())) * Rd(kYScale));

    // 0x00428671..0x00428689: uv = {[0x5d757c], [0x5d757c], [0x5cc320], [0x5cc320]} at [esp+0x1c..0x28]; the two
    // loaded values stay in ST for the blendMode stores below.
    const float zero = Rf(kZero);
    const float one  = Rf(kOne);
    float uv[4] = { zero, zero, one, one };

    // 0x0042868d..0x004286a1: w and h are rounded to float into [esp+0x14] / [esp+0x18].
    const float outW = static_cast<float>(static_cast<double>(sx) * static_cast<double>(w));
    const float outH = static_cast<float>(static_cast<double>(sy) * static_cast<double>(h));

    // 0x00428657..0x004286ab: coordMode 0 / 1 / 2; anything else jumps to 0x004286f3 with x and y still 0.
    if (coordMode == 0 || coordMode == 1) {
        // 0x004286db..0x004286ef
        outX = static_cast<float>(static_cast<double>(sx) * static_cast<double>(x));
        outY = static_cast<float>(static_cast<double>(sy) * static_cast<double>(y));
    } else if (coordMode == 2) {
        // 0x004286ad..0x004286c1: fld sx; fmul x; fld [esp+0x14]; fmul half; fsubp; fstp.
        outX = static_cast<float>(static_cast<double>(sx) * static_cast<double>(x)
                                  - static_cast<double>(outW) * Rd(kHalf));
        // 0x004286c5..0x004286ef, same chain on y / h.
        outY = static_cast<float>(static_cast<double>(sy) * static_cast<double>(y)
                                  - static_cast<double>(outH) * Rd(kHalf));
    }

    // 0x004286f3..0x00428720: ST0 = one, ST1 = zero.
    if (blendMode == 1) {            // 0x00428714
        uv[0] = one;
        uv[2] = zero;
    } else if (blendMode == 2) {     // 0x0042870a
        uv[1] = one;
        uv[3] = zero;
    } else if (blendMode == 3) {     // 0x00428700, then falls into 0x0042870a
        uv[0] = one;
        uv[2] = zero;
        uv[1] = one;
        uv[3] = zero;
    }

    // 0x0042865f xor ecx,ecx; 0x00428722..0x0042872a: tex handle = *tex when tex != NULL.
    std::uint32_t texv = 0u;
    if (tex != nullptr) texv = *tex;

    // 0x0042872c..0x0042874b: (tex, x, y, w, h, argb 0xffffffff from the byte stores 0x00428613..0x00428631, &uv).
    s_HudIm2DQuad(texv, outX, outY, outW, outH, 0xffffffffu, uv);
}

RH_ScopedInstall(ViewportScaledRectDraw, 0x00428610);
