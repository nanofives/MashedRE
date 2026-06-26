// Mashed RE — Render BGRA byte-reorder encoder (worktree wf_b0f68acd session).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004dfa40  BgraReorder4dfa40  — 34B; pure leaf; byte-permutation (BGRA -> ARGB word)
//
// Analysis notes:
//   re/analysis/bucket_004ddfb0/0x004dfa40.md
//
// Decompilation (cited from Ghidra, session pool2, 0x004dfa40):
//   undefined4 FUN_004dfa40(undefined1 *param_1)
//   {
//     return CONCAT31(CONCAT21(CONCAT11(param_1[3],*param_1),param_1[1]),param_1[2]);
//   }
//   body: 0x004dfa40 .. 0x004dfa61 (34 bytes)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// BgraReorder4dfa40  --  0x004dfa40
//
// Original: FUN_004dfa40 (34 bytes, 0x004dfa40..0x004dfa61)
// Signature: undefined4 FUN_004dfa40(undefined1 *param_1)
//   -> __cdecl; single pointer arg; returns uint32.
// Returns: byte-permutation of 4 input bytes:
//   result = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2]
//   (swaps channel indices 0 and 2; i.e. BGRA -> ARGB reinterpret, noted as D3DFMT_A8R8G8B8 conv)
//
// Constants: none (pure byte-permutation, no data references).
// Callees: none (pure leaf).
// Callers: FUN_004e02d0 (C2; render).
//
// Anti-island: leaf-function exemption applies (no callees).
//
// ref: re/analysis/bucket_004ddfb0/0x004dfa40.md
// ---------------------------------------------------------------------------

// 0x004dfa40
extern "C" __declspec(dllexport) std::uint32_t __cdecl BgraReorder4dfa40(
    const std::uint8_t* param_1)
{
    // CONCAT31(CONCAT21(CONCAT11(param_1[3], *param_1), param_1[1]), param_1[2])
    // cited from 0x004dfa40 decompilation:
    //   p[3] -> bits [31:24]
    //   p[0] -> bits [23:16]
    //   p[1] -> bits [15:8]
    //   p[2] -> bits [7:0]
    return (static_cast<std::uint32_t>(param_1[3]) << 24)
         | (static_cast<std::uint32_t>(param_1[0]) << 16)
         | (static_cast<std::uint32_t>(param_1[1]) <<  8)
         |  static_cast<std::uint32_t>(param_1[2]);
}

RH_ScopedInstall(BgraReorder4dfa40, 0x004dfa40);
