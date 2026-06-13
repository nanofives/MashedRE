// Mashed RE — promote-round round 12 (L3 looser-curation remainder).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0041b510  HudCounterReset      — hud; zero-store setter
//   0x00431b10  BootParamSet2        — boot; const-2 setter
//   0x004d6e60  TexStageCacheGet     — render; per-stage indexed getter
//
// DEFERRED this round: 0x004d71f0 (texturing-override flag getter) — all 3
// callers are third-party-library[renderware] rows held at C1 by policy;
// the C3 caller gate is unfillable unless a first-party caller surfaces.
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12.
//
// Analysis:
//   re/analysis/bucket_util_0040e4b0_0042f790/0x0041b510.md
//   re/analysis/skeleton_prep_high_leverage/00431b10.md
//   re/analysis/render_6_c1_to_c2_s2/004d6e60.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// HudCounterReset  --  0x0041b510   (subsystem: hud)
//
// Original: FUN_0041b510 (11 bytes, 0x0041b510..0x0041b51a)
// Bytes: C7 05 B0 CA 63 00 00 00 00 00 / C3
//   (mov dword ptr [0x0063cab0],0; ret)
// Signature: void FUN_0041b510(void)
//
// Constants (cited from function body at 0x0041b510):
//   0x0063cab0 — counter zeroed (incremented by FUN_0041b540 per the note)
//
// Caller: FUN_004111c0 (util, C2).
// ---------------------------------------------------------------------------

// 0x0041b510
extern "C" __declspec(dllexport) void __cdecl HudCounterReset(void) {
    // 0x0063cab0 := 0 cited at 0x0041b510 body.
    *reinterpret_cast<std::uint32_t*>(0x0063cab0u) = 0u;
}

RH_ScopedInstall(HudCounterReset, 0x0041b510);

// ---------------------------------------------------------------------------
// BootParamSet2  --  0x00431b10   (subsystem: boot)
//
// Original: FUN_00431b10 (11 bytes, 0x00431b10..0x00431b1a)
// Bytes: C7 05 10 0F 7F 00 02 00 00 00 / C3
//   (mov dword ptr [0x007f0f10],2; ret)
// Signature: void FUN_00431b10(void)
//
// Constants (cited at 0x00431b12):
//   0x007f0f10 — target global (Boot_DefaultParams_007f0f00 cluster, +0x10)
//   2          — literal written
//
// Caller: 0x004924f0 (boot, C3).
// ---------------------------------------------------------------------------

// 0x00431b10
extern "C" __declspec(dllexport) void __cdecl BootParamSet2(void) {
    // 0x007f0f10 := 2 cited at 0x00431b12.
    *reinterpret_cast<std::uint32_t*>(0x007f0f10u) = 2u;
}

RH_ScopedInstall(BootParamSet2, 0x00431b10);

// ---------------------------------------------------------------------------
// TexStageCacheGet  --  0x004d6e60   (subsystem: render)
//
// Original: FUN_004d6e60 (15 bytes, 0x004d6e60..0x004d6e6e)
// Bytes: 8B 44 24 04 / 8D 04 40 / 8B 04 C5 30 6B 7D 00 / C3
//   (mov eax,[esp+4]; lea eax,[eax+eax*2]; mov eax,[eax*8 + 0x007d6b30]; ret)
//   Effective byte stride: 3 * 8 = 24 (0x18) — "stride 6 dwords" per the note.
// Signature: undefined4 FUN_004d6e60(int param_1)
//
// No bounds check — vectors stay 0..7 (D3D stage range; BSS array).
//
// Constants (cited from function body, base at 0x004d6e67):
//   0x007d6b30 — per-stage texture cache base (write side FUN_004d6ce0)
//   0x18       — per-stage byte stride (lea *3 then *8 scaling)
//
// Callers: rwD3D8RasterDestroy 0x004d3090 (C2), FUN_004db770 (C2).
//
// Uncertainties (non-blocking):
//   U-5329: cache entry semantics (data-semantic).
// ---------------------------------------------------------------------------

// 0x004d6e60
extern "C" __declspec(dllexport) std::uint32_t __cdecl TexStageCacheGet(int param_1) {
    // 0x007d6b30 base + 0x18 byte stride cited at 0x004d6e60 body.
    return *reinterpret_cast<const std::uint32_t*>(
        0x007d6b30u + static_cast<std::uint32_t>(param_1) * 0x18u);
}

RH_ScopedInstall(TexStageCacheGet, 0x004d6e60);
