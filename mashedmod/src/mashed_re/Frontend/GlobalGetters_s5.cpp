// Mashed RE - Frontend global getter reimplementations (c3-batch-s session 5).
// Cluster: Frontend/GlobalGetters_s5.cpp
//
// Candidates: 0x004260a0, 0x004260b0, 0x00426bc0, 0x00426bd0
// Refused:    0x00426b40 (7 of 10 callees at C1 — anti-island rule)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis notes: re/analysis/frontend_c1_to_c2_s4/

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetDat00657438  --  0x004260a0
//
// Original: FUN_004260a0 (5 bytes, 0x004260a0..0x004260a5)
// Returns: DAT_00657438 (4-byte global, read and returned directly).
// No callees, no branches, no side-effects.
// Callers: FUN_00420420, FUN_00420870 (both also call FUN_004260b0 at 0x0065743c).
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_004260a0.md
// ---------------------------------------------------------------------------

// 0x004260a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat00657438() {
    return *reinterpret_cast<std::uint32_t*>(0x00657438u);
}

RH_ScopedInstall(GetDat00657438, 0x004260a0);

// ---------------------------------------------------------------------------
// GetDat0065743c  --  0x004260b0
//
// Original: FUN_004260b0 (5 bytes, 0x004260b0..0x004260b5)
// Returns: DAT_0065743c (4-byte global, read and returned directly).
// No callees, no branches, no side-effects.
// Adjacent to GetDat00657438 (reads 0x00657438); same two primary callers.
// 4 callers total: FUN_00420420, FUN_00420870, FUN_0044bfa0, FUN_00487280.
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_004260b0.md
// ---------------------------------------------------------------------------

// 0x004260b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0065743c() {
    return *reinterpret_cast<std::uint32_t*>(0x0065743cu);
}

RH_ScopedInstall(GetDat0065743c, 0x004260b0);

// ---------------------------------------------------------------------------
// GetDat0066d6e0  --  0x00426bc0
//
// Original: FUN_00426bc0 (5 bytes, 0x00426bc0..0x00426bc5)
// Returns: DAT_0066d6e0 (4-byte global, read and returned directly).
// No callees, no branches, no side-effects.
// Single caller: FUN_00409290.
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426bc0.md
// ---------------------------------------------------------------------------

// 0x00426bc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetDat0066d6e0() {
    return *reinterpret_cast<std::uint32_t*>(0x0066d6e0u);
}

RH_ScopedInstall(GetDat0066d6e0, 0x00426bc0);

// ---------------------------------------------------------------------------
// GetTableEntry0066d658  --  0x00426bd0
//
// Original: FUN_00426bd0 (34 bytes, 0x00426bd0..0x00426bf2)
// Reads a packed 4-byte record at global table DAT_0066d658 indexed by param_1.
// Stride: 4 bytes per record (param_1 * 4).
// Layout: bytes [0..1] = first uint16 field  -> written to *param_2
//         bytes [2..3] = second uint16 field -> written to *param_3
// Table base A: 0x0066d658 (low 2 bytes of each record)
// Table base B: 0x0066d65a (high 2 bytes of each record, = base + 2)
// No callees, no side-effects beyond writing to the two out-pointers.
// Callers: FUN_00409290 (same single caller as GetDat0066d6e0).
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426bd0.md
// ---------------------------------------------------------------------------

// 0x00426bd0
extern "C" __declspec(dllexport) void __cdecl GetTableEntry0066d658(
    int param_1,
    std::uint16_t* param_2,
    std::uint16_t* param_3)
{
    // Read two consecutive 16-bit fields from the packed 4-byte record at
    // table[param_1].  Each record occupies 4 bytes at stride * param_1.
    // DAT_0066d658 = low uint16; DAT_0066d65a (= +2) = high uint16.
    const std::uint8_t* base = reinterpret_cast<const std::uint8_t*>(0x0066d658u);
    const std::uint8_t* entry = base + static_cast<std::uint32_t>(param_1) * 4u;
    *param_2 = *reinterpret_cast<const std::uint16_t*>(entry);
    *param_3 = *reinterpret_cast<const std::uint16_t*>(entry + 2u);
}

RH_ScopedInstall(GetTableEntry0066d658, 0x00426bd0);
