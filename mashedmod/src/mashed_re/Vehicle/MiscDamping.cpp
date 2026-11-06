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

// MATCH-S11: the body of 0x0046c570 (VehicleDampVec3) moved to Match/MatchS11_0046c570.cpp,
// which compiles the byte-matched source re/match/s11/0046c570.c.
