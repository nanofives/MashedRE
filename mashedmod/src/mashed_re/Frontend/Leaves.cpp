// Mashed RE — Frontend leaf functions, c3-batch-k session 3.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x00408a50  PerCarRaceProgressGet  — per-car race-progress float getter
//
// Analysis: re/analysis/promote_c2_vehicle_lowrva/0x00408a50.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// PerCarRaceProgressGet  --  0x00408a50
//
// Original: FUN_00408a50 (16 bytes, 0x00408a50..0x00408a5f)
// Signature: float10 FUN_00408a50(int param_1)
//   param_1: per-car slot index (0–3 in practice)
// Returns *(float*)(&DAT_008a96e8 + param_1 * 0x30c).
//
// Constants (cited from function body at 0x00408a50):
//   0x008a96e8 — base address of the float field in the per-car block.
//   0x30c (780) — per-car block stride.
//
// No branches, no side-effects. Result zero-extended to float10 via FILD/FSTP
// in the original FPU sequence.
//
// Uncertainties (non-blocking for reimpl):
//   U-1292: subsystem tag (ai vs vehicle); does not affect the reimpl logic.
//   Whether the block at stride 0x30c is the entire PerCarRaceProgress struct
//   or a sub-field; the read address is exactly &DAT_008a96e8 + param_1*0x30c.
//
// Callers:
//   FUN_00408a70 (writer, same bucket)
//   FUN_00410d10 (race-end aggregator loop)
//
// ref: re/analysis/promote_c2_vehicle_lowrva/0x00408a50.md
// ---------------------------------------------------------------------------

// 0x00408a50
extern "C" __declspec(dllexport) float __cdecl PerCarRaceProgressGet(int param_1) {
    // Read *(float*)(0x008a96e8 + param_1 * 0x30c).
    // Base 0x008a96e8 cited at 0x00408a50 body; stride 0x30c cited at 0x00408a50 body.
    const std::uintptr_t base = 0x008a96e8u;
    const std::uintptr_t stride = 0x30cu;
    return *reinterpret_cast<const float*>(base + static_cast<unsigned>(param_1) * stride);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(PerCarRaceProgressGet, 0x00408a50);
