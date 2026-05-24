// Mashed RE — Vehicle damping helper (c3-batch-j-s3).
// Single-function cluster for 0x0046c570: 3-float in-place damping multiply
// against a global scalar.  Newly unblocked by the vec3_global_mul_observe
// arg_type (re/frida/diff_template.js, harness commit 656273b).
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// Global addresses (cited from re/analysis/util_c0_promote/0x0046c570.md):
//   DAT_00881f50/54/58 — three consecutive floats in per-vehicle block
//                        (stride 0x341 DWORDs).
//   _DAT_005ce264      — scalar damping factor (float).
// stride is 0x341 DWORDs = 0xD04 bytes — same as the kVehicleState arrays.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kVehicleBase_881f50 = 0x00881f50u;
static constexpr std::uintptr_t kVehicleBase_881f54 = 0x00881f54u;
static constexpr std::uintptr_t kVehicleBase_881f58 = 0x00881f58u;
static constexpr std::uintptr_t kDampingScalar_5ce264 = 0x005ce264u;
static constexpr std::uint32_t  kDWordStride         = 0x341u;

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046c570  VehicleDampVec3
// Ghidra decomp (74 bytes):
//   *(float *)(&DAT_00881f50 + idx * 0x341 * 4) =
//       *(float *)(&DAT_00881f50 + idx * 0x341 * 4) * _DAT_005ce264;
//   *(float *)(&DAT_00881f54 + idx * 0x341 * 4) =
//       *(float *)(&DAT_00881f54 + idx * 0x341 * 4) * _DAT_005ce264;
//   *(float *)(&DAT_00881f58 + idx * 0x341 * 4) =
//       *(float *)(&DAT_00881f58 + idx * 0x341 * 4) * _DAT_005ce264;
//   return 1;
// Pure leaf; constant-1 return.  Caller 0x00424eb0 (game-mode cleanup loop).
// [UNCERTAIN] whether the 3-float field is a velocity, force, or angular
// momentum vector — meaning-only; mechanical reimpl is exact.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleDampVec3(int vehicleIdx) {
    const auto step  = static_cast<std::uintptr_t>(vehicleIdx) * kDWordStride;
    float* const fx = reinterpret_cast<float*>(kVehicleBase_881f50 + step * 4u);
    float* const fy = reinterpret_cast<float*>(kVehicleBase_881f54 + step * 4u);
    float* const fz = reinterpret_cast<float*>(kVehicleBase_881f58 + step * 4u);
    const float k    = *reinterpret_cast<const float*>(kDampingScalar_5ce264);
    *fx = *fx * k;
    *fy = *fy * k;
    *fz = *fz * k;
    return 1;
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleDampVec3, 0x0046c570);
