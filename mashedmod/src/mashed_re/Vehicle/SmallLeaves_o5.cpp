// Mashed RE — Vehicle small-leaf C2->C3 promotions (c3-batch-o-s5).
// Two new vehicle-subsystem functions promoted from C2.
//
//   0x0046cbe0  VehicleCarStateSet     — per-vehicle spinout state setter
//   0x0042d3a0  RaceTransitionBufZero  — bulk-zero 832 bytes of transition buffer
//
// NOTE: 0x00467300 VehicleCollisionWinTrigger was found already implemented in
//       Vehicle/Damage.cpp with RH_ScopedInstall.  It is promoted separately
//       via hooks_registry entry + Frida diff only.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// Global addresses (all in .bss):
//
//   Spinout-state arrays (stride 0x341 DWORDs = 0xD04 bytes per vehicle slot):
//     0x00881f90  DAT_00881f90  — spinout state (uint8; 0=alive, 2=slide, 0xff=expired)
//     0x00881f94  DAT_00881f94  — secondary state/timer (uint32, stride 0xD04 bytes)
//
//   Race-transition parameter buffer:
//     0x0067ed78  — start of 832-byte (0x340) transition entry block
// ─────────────────────────────────────────────────────────────────────────────

static constexpr std::uintptr_t kSpinoutStateBase   = 0x00881f90u;  // DAT_00881f90
static constexpr std::uintptr_t kSecondaryStateBase  = 0x00881f94u; // DAT_00881f94
static constexpr std::uint32_t  kDWordStride         = 0x341u;       // per-vehicle DWord stride
static constexpr std::uint32_t  kByteStride          = 0xD04u;       // per-vehicle byte stride

static constexpr std::uintptr_t kRaceTransBufBase   = 0x0067ed78u;  // start of transition buf
static constexpr std::uint32_t  kRaceTransBufSize   = 0x340u;       // 832 bytes

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046cbe0  VehicleCarStateSet
// Setter companion to FUN_0046cbb0 (VehicleCarStateRead, C2).
//
// Ghidra decomp (43 bytes):
//   if (param_1 > 0xf) return 0;
//   (&DAT_00881f90)[param_1 * 0x341]              = (uint8)param_2;
//   *(uint32*)(DAT_00881f94 + param_1 * 0xd04)   = param_3;
//   return 1;
//
// param_1 = vehicle/player index (0–15); OOB guard > 0xf.
// param_2 = spinout state (0=alive, 2=slide, 0xff=expired per callers).
// param_3 = secondary state value (timer or secondary flag; U-1856 in getter).
// Caller: FUN_00424eb0 (C2) — game-mode cleanup loop.
// Pure leaf (callees=[]).  Leaf-function callee exemption applies.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleCarStateSet(int vehicleIdx, std::uint32_t spinoutState, std::uint32_t secondaryVal) {
    if (vehicleIdx > 0xf) return 0;
    // Spinout-state: byte write at base + vehicleIdx * DWordStride * 4.
    reinterpret_cast<std::uint8_t*>(kSpinoutStateBase)[vehicleIdx * kDWordStride] = static_cast<std::uint8_t>(spinoutState);
    // Secondary state: uint32 write at kSecondaryStateBase + vehicleIdx * kByteStride.
    *reinterpret_cast<std::uint32_t*>(kSecondaryStateBase + static_cast<std::uintptr_t>(vehicleIdx) * kByteStride) = secondaryVal;
    return 1;
}
RH_ScopedInstall(VehicleCarStateSet, 0x0046cbe0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0042d3a0  RaceTransitionBufZero
// Bulk-zeroes 832 bytes (13 × 64-byte entries) at 0x0067ed78..0x0067f0b7.
// Called by FUN_00432080 (C2) and FUN_004331a0 (C2) as a pre-commit clear.
//
// Ghidra decomp (52 bytes) — unrolled loop equivalent:
//   uint32_t* puVar2 = &DAT_0067ed7c;
//   do {
//     puVar2[-1] = 0;              // &DAT_0067ed78
//     *puVar2    = 0;              // &DAT_0067ed7c
//     puVar2[1]  = 0;              // &DAT_0067ed80
//     uint32_t* puVar3 = puVar2 + 2;
//     for (int i = 0xc; i != 0; i--) { *puVar3++ = 0; }  // 12 more dwords
//     puVar2[0xe] = 0;             // trailer dword (+0x38)
//     puVar2 += 0x10;              // advance 64 bytes (16 dwords)
//   } while ((int)puVar2 < 0x67f0bc);
//
// Semantically: memset(0x0067ed78, 0, 0x340).
// Outer loop: 13 iterations, stride 0x40 (64 bytes).
// Inner structure per slot: 3 explicit + 12-dword inner loop + 1 trailer = 16 dwords.
// S-1067/S-1068: writer/owner of cleared region unresolved (catalogued; not C3 blockers).
// Callers: FUN_00432080 (C2), FUN_004331a0 (C2).
// Pure leaf (callees=[]).  Leaf-function callee exemption applies.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl RaceTransitionBufZero(void) {
    std::memset(reinterpret_cast<void*>(kRaceTransBufBase), 0, kRaceTransBufSize);
}
RH_ScopedInstall(RaceTransitionBufZero, 0x0042d3a0);
