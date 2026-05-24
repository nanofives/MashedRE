// Mashed RE - Frontend menu initialisation reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_frontend_menus/0x0042fe80.md  (GetRaceEndFlag)
//   re/analysis/promote_c2_frontend_menus/0x0042f0b0.md  (GetFrameCounterPlus73)
//   re/analysis/promote_c2_frontend_menus/0x0042d3e0.md  (MenuEntryArrayInit)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetRaceEndFlag  --  0x0042fe80
//
// Original: FUN_0042fe80 (5 bytes, 0x0042fe80..0x0042fe85)
// Body: MOV EAX, [0x0067ea90]; RET
// Returns: DAT_0067ea90 (race-end flag or slot gate).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042fe80.md
// ---------------------------------------------------------------------------

// 0x0042fe80
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetRaceEndFlag() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ea90u);
}

RH_ScopedInstall(GetRaceEndFlag, 0x0042fe80);  // re-enabled 2026-05-24 batch-frontend

// ---------------------------------------------------------------------------
// GetFrameCounterPlus73  --  0x0042f0b0
//
// Original: FUN_0042f0b0 (8 bytes, 0x0042f0b0..0x0042f0b8)
// Body: MOV EAX, [0x0067f17c]; ADD EAX, 0x49; RET
// Returns: DAT_0067f17c (animation frame counter) + 0x49 (73, cited at 0x0042f0b4).
// No callees, no branches, no side-effects.
// ref: re/analysis/promote_c2_frontend_menus/0x0042f0b0.md
// ---------------------------------------------------------------------------

// 0x0042f0b0
extern "C" __declspec(dllexport) int __cdecl GetFrameCounterPlus73() {
    return *reinterpret_cast<std::int32_t*>(0x0067f17cu) + 0x49;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(GetFrameCounterPlus73, 0x0042f0b0);

// ---------------------------------------------------------------------------
// MenuEntryArrayInit  --  0x0042d3e0
//
// Original: FUN_0042d3e0 (58 bytes, 0x0042d3e0..0x0042d41a)
// Zeroes 30 entries of a 52-byte struct array starting at 0x00898ac0.
// Loop: start at DAT_00898ac4 (= array_base + 4, i.e. entry[0]+4).
//   Each iteration clears 14 selected dword-fields per entry (see below).
//   Stride: puVar1 += 0xd (13 dwords = 52 bytes per entry).
//   Termination: puVar1 >= 0x8990dc (exclusive end of 30-entry array).
//
// Zeroed offsets per entry (relative to entry base = puVar1 - 1 dword = -4):
//   -4, 0, +4, +8, +9, +10, +11, +12, +16, +20, +24, +32, +36, +40
// NOT zeroed: offsets +28 and any beyond +40.
//
// Implementation note: the loop pointer starts at entry[0]+4 (i.e. offset +4
// past the array base 0x00898ac0), so the "entry base" for offset indexing is
// puVar1 - 4.  The original iterates by adding 0xd uint32*'s (52 bytes) to
// the loop pointer each step.  We reproduce this exactly — do NOT simplify to
// memset, as +28 and any bytes beyond +40 must NOT be touched.
// ref: re/analysis/promote_c2_frontend_menus/0x0042d3e0.md
// ---------------------------------------------------------------------------

// 0x0042d3e0
extern "C" __declspec(dllexport) void __cdecl MenuEntryArrayInit() {
    // Loop pointer starts at array_base + 4 (= DAT_00898ac4).
    std::uint32_t* puVar1 = reinterpret_cast<std::uint32_t*>(0x00898ac4u);
    // Exclusive end: 0x008990dc.
    const std::uint32_t* const end = reinterpret_cast<const std::uint32_t*>(0x008990dcu);

    while (puVar1 < end) {
        // Entry base (byte pointer) = (char*)puVar1 - 4.
        // All offsets are relative to entry base.
        std::uint8_t* base = reinterpret_cast<std::uint8_t*>(puVar1) - 4;

        // Zero the 14 selected offsets: -4, 0, +4, +8, +9, +10, +11, +12,
        //                                +16, +20, +24, +32, +36, +40.
        *reinterpret_cast<std::uint32_t*>(base - 4)  = 0u;  // offset -4
        *reinterpret_cast<std::uint32_t*>(base +  0) = 0u;  // offset  0
        *reinterpret_cast<std::uint32_t*>(base +  4) = 0u;  // offset +4
        *reinterpret_cast<std::uint32_t*>(base +  8) = 0u;  // offset +8
        *reinterpret_cast<std::uint8_t*> (base +  9) = 0u;  // offset +9  (byte)
        *reinterpret_cast<std::uint8_t*> (base + 10) = 0u;  // offset +10 (byte)
        *reinterpret_cast<std::uint8_t*> (base + 11) = 0u;  // offset +11 (byte)
        *reinterpret_cast<std::uint8_t*> (base + 12) = 0u;  // offset +12 (byte)
        *reinterpret_cast<std::uint32_t*>(base + 16) = 0u;  // offset +16
        *reinterpret_cast<std::uint32_t*>(base + 20) = 0u;  // offset +20
        *reinterpret_cast<std::uint32_t*>(base + 24) = 0u;  // offset +24
        *reinterpret_cast<std::uint32_t*>(base + 32) = 0u;  // offset +32
        *reinterpret_cast<std::uint32_t*>(base + 36) = 0u;  // offset +36
        *reinterpret_cast<std::uint32_t*>(base + 40) = 0u;  // offset +40
        // NOT zeroed: +28, and any offsets beyond +40.

        // Advance by 0xd dwords = 52 bytes.
        puVar1 += 0xd;
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(MenuEntryArrayInit, 0x0042d3e0);
