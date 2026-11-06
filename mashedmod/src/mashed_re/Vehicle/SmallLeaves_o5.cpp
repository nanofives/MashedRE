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

// MATCH-S11: the body of 0x0046cbe0 (VehicleCarStateSet) moved to Match/MatchS11_0046cbe0.cpp,
// which compiles the byte-matched source re/match/s11/0046cbe0.c.

// MATCH-S07: the body of 0x0042d3a0 (RaceTransitionBufZero) moved to Match/MatchS07_0042d3a0.cpp,
// which compiles the byte-matched source re/match/s07/0042d3a0.c.
