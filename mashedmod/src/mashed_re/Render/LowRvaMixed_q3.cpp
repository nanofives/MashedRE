// Mashed RE — Render low-RVA mixed reimplementations (c3-batch-q session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00422af0  SlotWordSet         — 20B; writes param_2 to DAT_00641320+param_1*0xf40
//   0x00403d30  FUN_00403d30        — 123B; two FUN_00427ad0 calls (shadow+fill, id 0x22f)
//   0x00403ed0  FUN_00403ed0        — 205B; four FUN_00427ad0 calls (ids 0xf3+0x230 pairs)
//   0x0040df60  ConditionalRenderSubPass — 55B; 3-gate conditional → FUN_00401f10
//
// Analysis notes:
//   re/analysis/c0_promotion_render_a/0x00422af0.md
//   re/analysis/promote_c2_render_lowrva/00403d30.md
//   re/analysis/promote_c2_render_lowrva/00403ed0.md
//   re/analysis/promote_c2_render_lowrva/0040df60.md
//
// Deferred this session:
//   0x00422ac0 — void(int, ptr-to-4-array); no matching arg_type in harness for
//     (int, ptr→4dword) signature; needs new harness arg_type; D-filed.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees
// ---------------------------------------------------------------------------

// 0x00427ad0  FUN_00427ad0 (C3: MenuMenusBB) — 7-arg sprite/text draw dispatcher.
// Draws a sprite/text element with id, x, y, w, h, color, scale.
static auto* const s_FUN_00427ad0 =
    reinterpret_cast<void(__cdecl*)(int, float, float, float, float, std::uint32_t, float)>(0x00427ad0);

// 0x0042f6a0  GetRaceSubMode (C3) — returns DAT_0067e9fc; race-end phase register.
static auto* const s_GetRaceSubMode =
    reinterpret_cast<int(__cdecl*)(void)>(0x0042f6a0);

// 0x00401f10  FUN_00401f10 (C2) — RW state bracketed iteration over DAT_00636ac0 array.
static auto* const s_FUN_00401f10 =
    reinterpret_cast<void(__cdecl*)(void)>(0x00401f10);

// ---------------------------------------------------------------------------
// SlotWordSet  --  0x00422af0
//
// Original: FUN_00422af0 (20 bytes, 0x00422af0..0x00422b04)
// Signature: void FUN_00422af0(int param_1, undefined4 param_2)
// Returns: void
//
// Body (cited from 0x00422af0):
//   *(DAT_00641320 + param_1 * 0xf40) = param_2;
//
// Constants (cited from 0x00422af0 body):
//   0x00641320 — per-slot array base (write target)
//   0xf40 (3904) — per-slot stride in bytes (cited at ~0x00422af3)
//
// Callees: none (pure leaf).
// Callers: (tracked via hooks.csv; C2 sibling of 0x00422ac0).
//
// Anti-island: leaf-function exemption applies (no callees).
//
// ref: re/analysis/c0_promotion_render_a/0x00422af0.md
// ---------------------------------------------------------------------------

// 0x00422af0
extern "C" __declspec(dllexport) void __cdecl SlotWordSet(
    int param_1, std::uint32_t param_2)
{
    // Write param_2 to per-slot word at DAT_00641320 + param_1 * 0xf40.
    // Base 0x00641320, stride 0xf40 cited at 0x00422af0 body.
    *reinterpret_cast<std::uint32_t*>(0x00641320u + static_cast<unsigned>(param_1) * 0xf40u) = param_2;
}

RH_ScopedInstall(SlotWordSet, 0x00422af0);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// SlotQuadSet  --  0x00422ac0
//
// Original: FUN_00422ac0 (41 bytes, 0x00422ac0..0x00422ae9)
// Signature: void FUN_00422ac0(int param_1, undefined4 *param_2)
// Returns: void
//
// Body (cited from re/analysis/c0_promotion_render_a/0x00422ac0.md):
//   stride = param_1 * 0xf40                                  // 0x00422ac4
//   DAT_006412e8 + stride          = param_2[0]               // 0x00422ac9
//   DAT_006412ec + stride (+4)     = param_2[1]               // +0x4
//   DAT_006412f0 + stride (+8)     = param_2[2]               // +0x8
//   DAT_006412f4 + stride (+0xc)   = param_2[3]               // +0xc
//
// Constants (all cited from 0x00422ac0 body):
//   0x006412e8 — per-slot quad-write base
//   0xf40 (3904) — per-slot stride (same as SlotWordSet at 0x00422af0)
//
// Callees: none (leaf).
// Callers: (tracked via hooks.csv; C2 sibling of 0x00422af0).
//
// ref: re/analysis/c0_promotion_render_a/0x00422ac0.md
// ---------------------------------------------------------------------------

// 0x00422ac0
extern "C" __declspec(dllexport) void __cdecl SlotQuadSet(
    int param_1, const std::uint32_t* param_2)
{
    // stride = param_1 * 0xf40, cited at 0x00422ac4
    const unsigned stride = static_cast<unsigned>(param_1) * 0xf40u;
    // Base 0x006412e8, cited at 0x00422ac9
    auto* base = reinterpret_cast<std::uint32_t*>(0x006412e8u + stride);
    base[0] = param_2[0];   // DAT_006412e8 + stride + 0
    base[1] = param_2[1];   // DAT_006412ec + stride + 4
    base[2] = param_2[2];   // DAT_006412f0 + stride + 8
    base[3] = param_2[3];   // DAT_006412f4 + stride + 12
}

RH_ScopedInstall(SlotQuadSet, 0x00422ac0);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// FUN_00403d30  --  0x00403d30
//
// Original: FUN_00403d30 (123 bytes, 0x00403d30..0x00403dab)
// Signature: void FUN_00403d30(void)
// Returns: void
//
// Body (cited from 0x00403d30):
//   Two unconditional calls to FUN_00427ad0 (sprite/text draw dispatcher).
//   Call 1 (drop-shadow): id=0x22f, x=123.0f, y=83.0f, w=400.0f, h=80.0f,
//                         color=0xff000000, scale=0.8f
//   Call 2 (fill):        id=0x22f, x=120.0f, y=80.0f, w=400.0f, h=80.0f,
//                         color=0xc8ffffff, scale=0.8f
//
// Constants (cited from 0x00403d30 body):
//   0x22f (559)       — sprite/text id (both calls)
//   0x42f60000        — 123.0f (x_shadow)
//   0x42f00000        — 120.0f (x_fill)
//   0x42a60000        — 83.0f  (y_shadow)
//   0x42a00000        — 80.0f  (y_fill, also h)
//   0x43c80000        — 400.0f (width)
//   0xff000000        — opaque black (shadow color, A=0xFF R=G=B=0)
//   0xc8ffffff        — white α=200 (fill color)
//   0x3f4ccccd        — 0.8f   (scale)
//
// Pattern: standard 3-pixel drop-shadow (Δx=+3, Δy=+3).
//
// Callee: FUN_00427ad0 (C3: MenuMenusBB).
// Caller: FUN_00404320 (C2: PerModeRenderMachine) — mode-9 dispatch path.
//
// ref: re/analysis/promote_c2_render_lowrva/00403d30.md
// ---------------------------------------------------------------------------

// 0x00403d30
extern "C" __declspec(dllexport) void __cdecl Render_00403d30(void)
{
    // Call 1: drop-shadow (x=123, y=83, color=opaque black).
    // Constants cited from 0x00403d30 body.
    s_FUN_00427ad0(
        0x22f,          // sprite/text id [0x00403d30 body]
        123.0f,         // x_shadow [0x42f60000]
        83.0f,          // y_shadow [0x42a60000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xff000000u,    // color: opaque black [0xff000000]
        0.8f            // scale [0x3f4ccccd]
    );

    // Call 2: fill (x=120, y=80, color=semi-transparent white).
    // Constants cited from 0x00403d30 body.
    s_FUN_00427ad0(
        0x22f,          // sprite/text id [0x00403d30 body]
        120.0f,         // x_fill   [0x42f00000]
        80.0f,          // y_fill   [0x42a00000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xc8ffffffu,    // color: white α=200 [0xc8ffffff]
        0.8f            // scale [0x3f4ccccd]
    );
}

RH_ScopedInstall(Render_00403d30, 0x00403d30);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// FUN_00403ed0  --  0x00403ed0
//
// Original: FUN_00403ed0 (205 bytes, 0x00403ed0..0x00403f9d)
// Signature: void FUN_00403ed0(void)
// Returns: void
//
// Body (cited from 0x00403ed0):
//   Four unconditional calls to FUN_00427ad0 in two shadow+fill pairs.
//   Pair 1 (id 0xf3):
//     Shadow: x=123.0f, y=83.0f,  color=0xff000000, scale=0.8f
//     Fill:   x=120.0f, y=80.0f,  color=0xc8ffffff, scale=0.8f
//   Pair 2 (id 0x230):
//     Shadow: x=123.0f, y=113.0f, color=0xff000000, scale=0.8f
//     Fill:   x=120.0f, y=110.0f, color=0xc8ffffff, scale=0.8f
//   All 4 calls share w=400.0f, h=80.0f.
//
// Constants (cited from 0x00403ed0 body):
//   0xf3 (243)        — sprite id pair 1
//   0x230 (560)       — sprite id pair 2
//   0x42f60000        — 123.0f (x_shadow)
//   0x42f00000        — 120.0f (x_fill)
//   0x42a60000        — 83.0f  (y_shadow pair 1)
//   0x42a00000        — 80.0f  (y_fill   pair 1, also h)
//   0x42e20000        — 113.0f (y_shadow pair 2)
//   0x42dc0000        — 110.0f (y_fill   pair 2)
//   0x43c80000        — 400.0f (width)
//   0xff000000        — opaque black shadow color
//   0xc8ffffff        — white α=200 fill color
//   0x3f4ccccd        — 0.8f   (scale)
//
// Callee: FUN_00427ad0 (C3: MenuMenusBB).
// Caller: FUN_00404320 (C2: PerModeRenderMachine) — mode-5 dispatch path.
//
// ref: re/analysis/promote_c2_render_lowrva/00403ed0.md
// ---------------------------------------------------------------------------

// 0x00403ed0
extern "C" __declspec(dllexport) void __cdecl Render_00403ed0(void)
{
    // Pair 1 — id 0xf3, y positions 83/80. Constants cited from 0x00403ed0 body.

    // Call 1a: drop-shadow.
    s_FUN_00427ad0(
        0xf3,           // sprite id [0x00403ed0 body: 0xf3]
        123.0f,         // x_shadow [0x42f60000]
        83.0f,          // y_shadow [0x42a60000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xff000000u,    // opaque black [0xff000000]
        0.8f            // scale [0x3f4ccccd]
    );

    // Call 1b: fill.
    s_FUN_00427ad0(
        0xf3,           // sprite id [0x00403ed0 body: 0xf3]
        120.0f,         // x_fill   [0x42f00000]
        80.0f,          // y_fill   [0x42a00000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xc8ffffffu,    // white α=200 [0xc8ffffff]
        0.8f            // scale [0x3f4ccccd]
    );

    // Pair 2 — id 0x230, y positions 113/110. Constants cited from 0x00403ed0 body.

    // Call 2a: drop-shadow.
    s_FUN_00427ad0(
        0x230,          // sprite id [0x00403ed0 body: 0x230]
        123.0f,         // x_shadow [0x42f60000]
        113.0f,         // y_shadow [0x42e20000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xff000000u,    // opaque black [0xff000000]
        0.8f            // scale [0x3f4ccccd]
    );

    // Call 2b: fill.
    s_FUN_00427ad0(
        0x230,          // sprite id [0x00403ed0 body: 0x230]
        120.0f,         // x_fill   [0x42f00000]
        110.0f,         // y_fill   [0x42dc0000]
        400.0f,         // width    [0x43c80000]
        80.0f,          // height   [0x42a00000]
        0xc8ffffffu,    // white α=200 [0xc8ffffff]
        0.8f            // scale [0x3f4ccccd]
    );
}

RH_ScopedInstall(Render_00403ed0, 0x00403ed0);  // re-enabled 2026-05-24 c3-render-a

// ---------------------------------------------------------------------------
// ConditionalRenderSubPass  --  0x0040df60
//
// Original: sub_0040df60 (55 bytes, 0x0040df60..0x0040df97)
// Signature: void sub_0040df60(void)
// Returns: void
//
// Body (cited from 0x0040df60):
//   if ((4 < DAT_0063ba8c) && (DAT_0063ba8c < 7)) {
//       iVar1 = FUN_0042f6a0();            // GetRaceSubMode
//       if ((2 < iVar1) && (iVar1 < 6)) {
//           switch (DAT_007f0fd0) {
//           case 4: case 7: case 8: case 9: case 10:
//               FUN_00401f10();
//               return;
//           }
//       }
//   }
//   return;
//
// Gates (all must pass to reach FUN_00401f10):
//   1. DAT_0063ba8c ∈ (4, 7)  — exclusive; active values {5, 6}
//   2. FUN_0042f6a0() ∈ (2, 6) — exclusive; active values {3, 4, 5}
//   3. DAT_007f0fd0 ∈ {4, 7, 8, 9, 10} — explicit switch cases
//
// Constants (cited from 0x0040df60 body):
//   0x0063ba8c — player-count/mode config global; gates on ∈ {5, 6}
//   0x007f0fd0 — game-mode selector; active modes {4, 7, 8, 9, 10}
//   4, 7 — exclusive bounds for gate 1
//   2, 6 — exclusive bounds for gate 2
//
// Callees: FUN_0042f6a0 (C3: GetRaceSubMode), FUN_00401f10 (C2).
// Callers: per game-mode dispatch infrastructure.
//
// Uncertainties (non-blocking):
//   U-1713: semantic of DAT_0063ba8c ∈ {5,6} — phase ID vs player count.
//
// ref: re/analysis/promote_c2_render_lowrva/0040df60.md
// ---------------------------------------------------------------------------

// 0x0040df60
extern "C" __declspec(dllexport) void __cdecl ConditionalRenderSubPass(void)
{
    // Gate 1: DAT_0063ba8c must be in exclusive range (4, 7) — i.e. 5 or 6.
    // Address 0x0063ba8c cited at 0x0040df60 body.
    const int dat_0063ba8c = *reinterpret_cast<const int*>(0x0063ba8cu);
    if (!((4 < dat_0063ba8c) && (dat_0063ba8c < 7))) {
        return;
    }

    // Gate 2: GetRaceSubMode() must return value in exclusive range (2, 6) — i.e. 3, 4, or 5.
    // FUN_0042f6a0 (C3: GetRaceSubMode) cited at 0x0040df60 body.
    const int iVar1 = s_GetRaceSubMode();
    if (!((2 < iVar1) && (iVar1 < 6))) {
        return;
    }

    // Gate 3: DAT_007f0fd0 must be one of {4, 7, 8, 9, 10}.
    // Address 0x007f0fd0 cited at 0x0040df60 body.
    const int dat_007f0fd0 = *reinterpret_cast<const int*>(0x007f0fd0u);
    switch (dat_007f0fd0) {
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
        // All gates pass — dispatch to inner render sub-pass.
        // FUN_00401f10 (C2) cited at 0x0040df60 body.
        s_FUN_00401f10();
        return;
    default:
        // Mode not in active set — no-op.
        break;
    }
}

RH_ScopedInstall(ConditionalRenderSubPass, 0x0040df60);  // re-enabled 2026-05-24 c3-render-a
