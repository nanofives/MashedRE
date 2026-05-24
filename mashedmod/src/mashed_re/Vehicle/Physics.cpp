// Mashed RE - Vehicle physics leaf cluster (c3_batch_j4): 1 function.
//
// Promoted candidate from Session 4 of c3_batch_j:
//   0x0046cbe0  VehicleSpinoutStateSet         (util_c0_promote)
//
// Other Session-4 physics/update_d3 candidates refused (see PROMOTION_QUEUE.md):
//   0x0046c5f0  EAX+ESI custom calling conv — needs new arg_type
//   0x0046c7d0  callee-gate: 3 [UNCERTAIN] callees
//   0x0046ddb0  callee-gate + still C1 + size 3104b
//   0x0046dc20  bare U-2692 structural (uninitialised local_40)
//   0x0046f6c0  callee-gate (multiple unresolved stubs) + size 3580b
//
// Analysis source:
//   re/analysis/util_c0_promote/0x0046cbe0.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>


// ============================================================================
// 0x0046cbe0  VehicleSpinoutStateSet
//
// Original 43 bytes. Leaf — no callees.
//
// Body (Ghidra decomp, cited at 0x0046cbe0..0x0046cc0a):
//   if (0xf < param_1) return 0;
//   (&DAT_00881f90)[param_1 * 0x341]              = param_2;   // dword stride 0x341
//   *(uint32*)(&DAT_00881f94 + param_1 * 0xd04)   = param_3;   // byte  stride 0xd04
//   return 1;
//
// Companion setter to FUN_0046cbb0 (VehicleCarStateRead, C3) which reads the
// same two fields. Stride pair (0x341 dword vs 0xd04 byte) matches.
// Returns 1 on success, 0 on OOB.
// ============================================================================

// Same struct base as VehicleCarStateRead (read-side):
//   re/analysis/vehicle_promote_c2_a/0x0046cbb0.md
static constexpr std::uintptr_t kVehicleBase_881f90 = 0x00881f90u;
static constexpr std::uintptr_t kVehicleBase_881f94 = 0x00881f94u;
static constexpr std::uint32_t  kDWordStride         = 0x341u;
static constexpr std::uint32_t  kByteStride          = 0xd04u;

// 0x0046cbe0
extern "C" __declspec(dllexport)
std::uint32_t __cdecl VehicleSpinoutStateSet(std::uint32_t vehicleIdx,
                                              std::uint32_t state,
                                              std::uint32_t secondary)
{
    // Cited at 0x0046cbe0 body entry: OOB guard.
    if (vehicleIdx > 0xfu) return 0u;
    // Cited at body: dword-stride write (param_1 * 0x341 indexed as int32*).
    reinterpret_cast<std::uint32_t*>(kVehicleBase_881f90)[vehicleIdx * kDWordStride] = state;
    // Cited at body: byte-stride write (param_1 * 0xd04 added to byte base).
    *reinterpret_cast<std::uint32_t*>(kVehicleBase_881f94 + vehicleIdx * kByteStride) = secondary;
    return 1u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleSpinoutStateSet, 0x0046cbe0);
