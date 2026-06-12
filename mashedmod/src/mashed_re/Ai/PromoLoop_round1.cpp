// Mashed RE — promote-round round 1 (L0: c3_batch_race1 session-1 set).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00408af0  AiVehicleFieldPtrGet    — ai;  pointer-return field getter
//   0x00442cc0  AiVehicleFloat4Get      — ai;  bounds-checked per-vehicle float getter
//   0x00414030  AiSplineBankTimerReset  — ai;  spline-bank timer reset (writes 1000)
//   0x0040e350  GetRenderSubMode        — ai (used by vehicle/render too); global getter
//   0x00429300  HudOverlayFloatGet      — hud; float getter (kept here as the
//                                         round's cluster file; subsystem stays hud)
//
// Verification lane: scenario:'race' diffs (re/analysis/scenario_attach_lane.md).
//
// Analysis:
//   re/analysis/bucket_ai_00407a40_00415880/0x00408af0.md
//   re/analysis/bucket_ai_00415d00_00452ea0/0x00442cc0.md
//   re/analysis/bucket_ai_00407a40_00415880/0x00414030.md
//   re/analysis/promote_c2_vehicle_lowrva/0x0040e350.md
//   re/analysis/localization_d2/0x00429300.md

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

// ---------------------------------------------------------------------------
// AiSplineBankTimerReset  --  0x00414030
//
// Original: FUN_00414030 (47 bytes, 0x00414030..0x0041405f)
// Signature: void FUN_00414030(int param_1)
//   param_1: vehicle slot index, or -1 for "all"
// Body:
//   if (param_1 == -1) {
//     for (puVar1 = &DAT_008032d4; (int)puVar1 < 0x803324; puVar1 += 5)
//       *puVar1 = 1000;
//     return;
//   }
//   (&DAT_008032d4)[param_1 * 5] = 1000;
//
// Constants (cited from function body at 0x00414030):
//   0x008032d4 — timer table base; per-slot stride 5 DWORDs (0x14 bytes)
//   0x00803324 — "all" loop upper bound (exclusive); exactly 5 slots
//   0x3e8      — reset value 1000 written per slot
//
// Caller: FUN_00417180 (AI line-bank switches).
//
// Uncertainties (non-blocking):
//   U-7564: unit/role of the table value 1000 (data-semantic).
// ---------------------------------------------------------------------------

// 0x00414030
extern "C" __declspec(dllexport) void __cdecl AiSplineBankTimerReset(int param_1) {
    if (param_1 == -1) {
        // 0x008032d4 base, 0x00803324 bound, stride 5 dwords, value 1000
        // cited at 0x00414030 body.
        std::uint32_t* p = reinterpret_cast<std::uint32_t*>(0x008032d4u);
        while (reinterpret_cast<std::uintptr_t>(p) < 0x00803324u) {
            *p = 1000u;
            p += 5;
        }
        return;
    }
    // single-slot write cited at 0x00414030 body.
    reinterpret_cast<std::uint32_t*>(0x008032d4u)[param_1 * 5] = 1000u;
}

RH_ScopedInstall(AiSplineBankTimerReset, 0x00414030);

// ---------------------------------------------------------------------------
// GetRenderSubMode  --  0x0040e350
//
// Original: FUN_0040e350 (5 bytes, 0x0040e350..0x0040e355)
// Signature: undefined4 FUN_0040e350(void)
// Body: return DAT_0063ba8c;
//
// Constants (cited from function body at 0x0040e350):
//   0x0063ba8c — game-mode state global (FUN_00411170 writes 7 at "race over";
//                other observed compares: 4, 5, 8, 9)
//
// Name matches the fn-ptr binding citation in Render/PerModeRender.cpp:43
// (that binding is a file-local static, not an export — no clash).
// ---------------------------------------------------------------------------

// 0x0040e350
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRenderSubMode(void) {
    // 0x0063ba8c cited at 0x0040e350 body.
    return *reinterpret_cast<const std::uint32_t*>(0x0063ba8cu);
}

RH_ScopedInstall(GetRenderSubMode, 0x0040e350);

// ---------------------------------------------------------------------------
// HudOverlayFloatGet  --  0x00429300   (subsystem: hud)
//
// Original: FUN_00429300 (bytes DB 05 B8 91 89 00 C3 at 0x00429300 =
//   FILD dword ptr [0x008991B8]; RET — verified against
//   original\MASHED.exe.unpatched file offset 0x29300, 2026-06-12)
// Signature: float10 FUN_00429300(void)
// Body: return (float)(int)DAT_008991b8;   // SIGNED-INT load, x87 convert
//
// NOTE: the global holds an INTEGER (FILD converts int32 -> float). A raw
// float load here is WRONG (caught RED in the race diff: orig=923.0f vs
// denormal-bits-923 from the reimpl).
//
// The caller (FUN_00429620) passes an arg the body never reads (decomp shows
// void params; calling-convention mismatch noted in the analysis). __cdecl is
// caller-clean, so the ignored arg is ABI-safe.
//
// Constants (cited from instruction at 0x00429300):
//   0x008991b8 — int32 source (written at race-overlay setup; U-2279 suggests
//                a counter/timer — consistent with the observed value 923)
//
// Uncertainties (non-blocking):
//   U-2279: meaning of the value (data-semantic).
// ---------------------------------------------------------------------------

// 0x00429300
extern "C" __declspec(dllexport) float __cdecl HudOverlayFloatGet(void) {
    // FILD [0x008991b8] cited at 0x00429300 (instruction bytes above).
    return static_cast<float>(*reinterpret_cast<const std::int32_t*>(0x008991b8u));
}

RH_ScopedInstall(HudOverlayFloatGet, 0x00429300);
