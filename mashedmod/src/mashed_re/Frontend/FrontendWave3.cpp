// Mashed RE — Frontend subsystem Wave 3 mixed-candidate cluster.
// Session: wave3-s4   Branch: c3-sweep/wave3-s4
//
// Two frontend utility functions authored to C2 quality:
//   0x004298c0  ResetTimeTriplet   — zero 4 time-state globals
//   0x00414120  ScoreTableReset    — zero+copy 0x9c-byte score arrays
//
// Analysis notes:
//   re/analysis/c0_promotion_frontend_a/0x004298c0.md
//   re/analysis/c0_promotion_frontend_a/0x00414120.md
//
// Binary anchor:
//   original\MASHED.exe  size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

// ===========================================================================
// 0x004298c0  ResetTimeTriplet
//
// Zero out 4 time-state globals. No parameters, no return value.
// Body 27 bytes (0x004298c0–0x004298db). Pure leaf, no callees, no branches.
//
// Globals zeroed (from c0_promotion_frontend_a/0x004298c0.md):
//   _DAT_0067d99c = 0   (float-related; indexed by FUN_00429a80/90/70 per D-2765 context)
//    DAT_0067d994 = 0
//    DAT_0067d98c = 0
//    DAT_008991bc = 0   (scalar)
//
// [UNCERTAIN U-3433]: 0x0067d98c/d994/d99c are indexed by FUN_00429a80/90/70
//   (hud_frontend_d3 session); full struct layout/size not confirmed.
//   Non-blocking — the zero-write is mechanically unambiguous.
// ===========================================================================

// 0x004298c0
extern "C" __declspec(dllexport) void __cdecl ResetTimeTriplet() {
    *reinterpret_cast<std::uint32_t*>(0x0067d99cu) = 0u; // @0x004298c0 _DAT_0067d99c
    *reinterpret_cast<std::uint32_t*>(0x0067d994u) = 0u; // @0x004298c5  DAT_0067d994
    *reinterpret_cast<std::uint32_t*>(0x0067d98cu) = 0u; // @0x004298ca  DAT_0067d98c
    *reinterpret_cast<std::uint32_t*>(0x008991bcu) = 0u; // @0x004298cf  DAT_008991bc
}

RH_ScopedInstall(ResetTimeTriplet, 0x004298c0);

// ===========================================================================
// 0x00414120  ScoreTableReset
//
// Zeroes a 0x9c-byte array and copies a 0x9c-byte ROM table into a parallel
// working array. Body 80 bytes (0x00414120–0x00414170). No callees.
//
// Pre-loop: zero 3 scalar globals.
//   _DAT_0089a37c = 0   (0x0089a37c)
//    DAT_0089a378 = 0   (0x0089a378)
//   _DAT_0089a380 = 0   (0x0089a380)
//
// Loop structure (from 0x00414120.md):
//   outer: while iVar1 < 0x9c; inner: 3 iterations per outer, iVar1 += 4.
//   per step:
//     *(uint32*)(DAT_0089a420 + iVar1) = 0                             (zero destination)
//     *(uint32*)(DAT_0089a384 + iVar1) = *(uint32*)(DAT_005f2a70 + iVar1)  (copy from ROM)
//
// Net effect:
//   memset  (0x0089a420, 0,      0x9c)
//   memcpy  (0x0089a384, 0x005f2a70, 0x9c)
//
// [UNCERTAIN U-3432]: struct/array identity of 0x0089a384/0x0089a420/0x005f2a70
//   not documented. Non-blocking — copy/zero is mechanical.
// ===========================================================================

// 0x00414120
extern "C" __declspec(dllexport) void __cdecl ScoreTableReset() {
    // Pre-loop: zero 3 scalar globals
    *reinterpret_cast<std::uint32_t*>(0x0089a37cu) = 0u; // @0x00414120 _DAT_0089a37c
    *reinterpret_cast<std::uint32_t*>(0x0089a378u) = 0u; // @0x00414127  DAT_0089a378
    *reinterpret_cast<std::uint32_t*>(0x0089a380u) = 0u; // @0x0041412e _DAT_0089a380

    // Loop: 39 dwords (0x9c bytes = 156 bytes) visited as outer*3 inner steps
    // Reproduced as a single flat loop over dword offsets 0, 4, ..., 0x98.
    std::uint8_t* const dst_zero = reinterpret_cast<std::uint8_t*>(0x0089a420u);
    std::uint8_t* const dst_copy = reinterpret_cast<std::uint8_t*>(0x0089a384u);
    const std::uint8_t* const src_rom = reinterpret_cast<const std::uint8_t*>(0x005f2a70u);

    for (int iVar1 = 0; iVar1 < 0x9c; iVar1 += 4) {
        *reinterpret_cast<std::uint32_t*>(dst_zero + iVar1) = 0u;
        *reinterpret_cast<std::uint32_t*>(dst_copy + iVar1) =
            *reinterpret_cast<const std::uint32_t*>(src_rom + iVar1);
    }
}

RH_ScopedInstall(ScoreTableReset, 0x00414120);
