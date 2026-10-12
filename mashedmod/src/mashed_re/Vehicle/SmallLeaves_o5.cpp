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
//   Per-vehicle record, stride 0xD04 bytes (= 0x341 DWORDs), 0x0046cbf0 imul eax,eax,0xd04:
//     0x00881f90  DAT_00881f90  — DWORD stored at 0x0046cbfa
//     0x00881f94  DAT_00881f94  — DWORD stored at 0x0046cc00
//
//   Race-transition parameter buffer:
//     0x0067ed78  — start of 832-byte (0x340) transition entry block
// ─────────────────────────────────────────────────────────────────────────────

static constexpr std::uintptr_t kCarStateBase       = 0x00881f90u;  // 0x0046cbfa
static constexpr std::uintptr_t kCarStateBase2      = 0x00881f94u;  // 0x0046cc00
static constexpr std::uint32_t  kByteStride         = 0xD04u;       // 0x0046cbf0

static constexpr std::uintptr_t kRaceTransBufBase   = 0x0067ed78u;  // start of transition buf
static constexpr std::uint32_t  kRaceTransBufSize   = 0x340u;       // 832 bytes

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046cbe0  VehicleCarStateSet
// Setter companion to FUN_0046cbb0 (VehicleCarStateRead, C2).
//
// Disasm 0x0046cbe0..0x0046cc0b (cdecl, 3 stack args, 43 bytes):
//   0x0046cbe0  mov eax,[esp+4]           ; idx
//   0x0046cbe4  cmp eax,0x10 ; jb         ; UNSIGNED: idx >= 0x10 -> 0x0046cbe9 xor eax,eax ; ret
//   0x0046cbec  mov ecx,[esp+8]           ; a
//   0x0046cbf0  imul eax,eax,0xd04
//   0x0046cbf6  mov edx,[esp+0xc]         ; b
//   0x0046cbfa  mov [eax+0x881f90],ecx    ; DWORD store
//   0x0046cc00  mov [eax+0x881f94],edx    ; DWORD store
//   0x0046cc06  mov eax,1 ; ret
// Caller: FUN_00424eb0.  Pure leaf (no calls).
// This is the single implementation: the duplicate in Vehicle/Physics.cpp
// (VehicleSpinoutStateSet, which registered first and so was the copy actually
// installed in the .asi) was removed 2026-10-12 (reearn/fstate).
// ─────────────────────────────────────────────────────────────────────────────
// 0x0046cbe0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleCarStateSet(std::uint32_t vehicleIdx, std::uint32_t a, std::uint32_t b) {
    if (vehicleIdx >= 0x10u) return 0;                                                  // 0x0046cbe4 cmp/jb (unsigned)
    *reinterpret_cast<std::uint32_t*>(kCarStateBase  + vehicleIdx * kByteStride) = a;   // 0x0046cbfa
    *reinterpret_cast<std::uint32_t*>(kCarStateBase2 + vehicleIdx * kByteStride) = b;   // 0x0046cc00
    return 1;                                                                           // 0x0046cc06
}
RH_ScopedInstall(VehicleCarStateSet, 0x0046cbe0);  // re-ported 2026-10-12 reearn/fstate (dword store, unsigned guard)

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
RH_ScopedInstall(RaceTransitionBufZero, 0x0042d3a0);  // re-enabled 2026-05-24 c3-vehicle
