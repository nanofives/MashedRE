// Mashed RE — two AI per-vehicle field getters: ONE body each for both build targets (D4-DC-INC-ai, 2026-11-01).
//
// Before this file each RVA had two bodies, the dual-copy class (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md,
// ROADMAP D4):
//   Ai/PromoLoop_round1.cpp     .asi-only, installed, the body hooks.csv's C3 evidence covers
//   AL_408af0 (Ai/AiLeafTargets_naked.inc) / TG_442cc0 (Ai/AiModeSeven_naked.inc)
//                               exe-only, private verbatim naked copies called by the AI leaves
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The bodies moved here unchanged from
// Ai/PromoLoop_round1.cpp; the exe's naked callers now `call` them (re/tools/naked_specs/aileaf_g1.json,
// mode7_m4.json). RH_ScopedInstall installs them in the .asi and is a no-op in the exe.
//
// Exe equivalence with the deleted copies: both read only the absolute addresses the bytes name
// (0x008a96dc + v*0x30c is returned, never read; 0x008989b0 + v*4 and 0x005d757c are loaded). The exe maps
// 0x00500000..0x009fffff blank, so the 0.0 at 0x005d757c reads 0 there, the same bits TG_442cc0's exe-safe
// copy g_tg_c_5d757c (0x00000000) held.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis:
//   re/analysis/bucket_ai_00407a40_00415880/0x00408af0.md
//   re/analysis/bucket_ai_00415d00_00452ea0/0x00442cc0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// AiVehicleFieldPtrGet  --  0x00408af0
//
// Original: FUN_00408af0 (15 bytes, 0x00408af0..0x00408aff)
// Signature: undefined * FUN_00408af0(int param_1)
//   param_1: vehicle index
// Body:
//   return &DAT_008a96dc + param_1 * 0x30c;
//
// POINTER-RETURN getter: returns the ORIGINAL-image VA as a constant
// expression; never dereferences. Bit-identity compares the returned address.
//
// Constants (cited from function body at 0x00408af0):
//   0x008a96dc — field base inside per-vehicle record (= 0x008a9640 + 0x9c)
//   0x30c      — per-vehicle record stride (780 bytes)
//
// Callers: FUN_00415220, FUN_00415880, FUN_00416250, FUN_00416a30, FUN_00417da0.
//
// Uncertainties (non-blocking):
//   U-7562: meaning of the float3 at +0x9c (data-semantic).
// ---------------------------------------------------------------------------

// 0x00408af0
extern "C" __declspec(dllexport) std::uint32_t __cdecl AiVehicleFieldPtrGet(int param_1) {
    // 0x008a96dc base + 0x30c stride cited at 0x00408af0 body.
    return 0x008a96dcu + static_cast<std::uint32_t>(param_1) * 0x30cu;
}

RH_ScopedInstall(AiVehicleFieldPtrGet, 0x00408af0);

// ---------------------------------------------------------------------------
// AiVehicleFloat4Get  --  0x00442cc0
//
// Original: FUN_00442cc0 (23 bytes, 0x00442cc0..0x00442cd6)
// Signature: float10 FUN_00442cc0(int param_1)
//   param_1: vehicle index
// Body:
//   if (param_1 < 4) return *(float *)(&DAT_008989b0 + param_1 * 4);
//   return (float10)DAT_005d757c;
//
// Constants (cited from function body at 0x00442cc0):
//   0x008989b0 — base of 4-entry per-vehicle float array (stride 4)
//   0x005d757c — project-wide 0.0 float; returned for param_1 >= 4
//   4          — vehicle-count bound
//
// Callers: FUN_004148b0, FUN_00414c30, FUN_00415020, FUN_00415220,
//          FUN_00415190, FUN_00415200.
//
// Uncertainties (non-blocking):
//   U-7601: semantic of the stored float (data-semantic).
// ---------------------------------------------------------------------------

// 0x00442cc0
extern "C" __declspec(dllexport) float __cdecl AiVehicleFloat4Get(int param_1) {
    // 4 bound cited at 0x00442cc0 body (signed compare).
    if (param_1 < 4) {
        // 0x008989b0 base, stride 4 cited at 0x00442cc0 body.
        return *reinterpret_cast<const float*>(
            0x008989b0u + static_cast<std::uint32_t>(param_1) * 4u);
    }
    // 0x005d757c (0.0 float) cited at 0x00442cc0 body.
    return *reinterpret_cast<const float*>(0x005d757cu);
}

RH_ScopedInstall(AiVehicleFloat4Get, 0x00442cc0);
