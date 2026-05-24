// Mashed RE — Render D3D9 wrapper helpers (c3-batch-p session 4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004c8650  rwD3D9RenderStateCacheInvalidate — 64B; cache sentinel reset
//   0x004dcf90  rwD3D9CheckMMX                  — 87B; CPUID bit-23 query
//   0x004dcff0  rwD3D9CheckSSE_SSE2              — 91B; CPUID bits-25/26 query
//   0x004dd050  rwD3D9CheckSSE2                  — 86B; CPUID bit-26 query
//
// Deferred (live heap mutation via vtable free calls — no safe synthetic harness):
//   0x004cc9f0  RwFreeListDestroy — 136B; vtbl+0x10c/+0x11c raw free; global list unlink
//
// Analysis notes:
//   re/analysis/promote_c2_render_d3d9/0x004c8650.md
//   re/analysis/promote_c2_render_d3d9/0x004dcf90.md
//   re/analysis/promote_c2_render_d3d9/0x004dcff0.md
//   re/analysis/promote_c2_render_d3d9/0x004dd050.md
//   re/analysis/promote_c2_render_d3d9/0x004cc9f0.md

#include "../Core/HookSystem.h"

#include <cstdint>
#include <intrin.h>   // __cpuid

// ---------------------------------------------------------------------------
// rwD3D9RenderStateCacheInvalidate  --  0x004c8650
//
// Original: FUN_004c8650 (64 bytes, 0x004c8650..0x004c868f)
// Signature: void FUN_004c8650(void)
//
// Initialises two D3D9 render-state cache regions with sentinel values.
//
// Scalar cache (5 DWORDs at 0x006181c8..0x006181d8):
//   All written to 0xFFFFFFFF  — "invalid / not cached" sentinel.
//   [0x006181c8] = 0xFFFFFFFF
//   [0x006181cc] = 0xFFFFFFFF
//   [0x006181d0] = 0xFFFFFFFF
//   [0x006181d4] = 0xFFFFFFFF
//   [0x006181d8] = 0xFFFFFFFF
//
// Array cache at 0x007d40c0 (4 entries x stride 0x10 = 0x40 bytes total):
//   loop: uVar1 = 0; uVar1 < 0x40; uVar1 += 0x10   [0x004c8650 body]
//     [entry+0x00] = 0xFFFFFFFF  (sampler/texture handle sentinel)
//     [entry+0x04] = 0x00000000
//     [entry+0x08] = 0x00000000
//
// RenderWare D3D9 backend pattern: _rwD3D9RenderStateInvalidate / texture-stage-state reset.
// 0xFFFFFFFF is the universal RW D3D9 "dirty/unset" sentinel for cached render state.
//
// Constants (cited from 0x004c8650 body):
//   0x006181c8 — scalar render-state cache base (5 entries; offsets c8,cc,d0,d4,d8)
//   0x007d40c0 — array render-state cache base (4 entries x stride 0x10)
//   0x40 (64)  — total byte count of array cache
//   0x10 (16)  — stride per array entry
//
// Anti-island: callee=none (pure leaf). Callers at C2+ in render subsystem.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004c8650.md
// ---------------------------------------------------------------------------

// 0x004c8650
extern "C" __declspec(dllexport) void __cdecl rwD3D9RenderStateCacheInvalidate()
{
    // Scalar cache: 5 DWORDs at 0x006181c8..0x006181d8 written to 0xFFFFFFFF. [0x004c8650 body]
    *reinterpret_cast<std::uint32_t*>(0x006181c8u) = 0xFFFFFFFFu;
    *reinterpret_cast<std::uint32_t*>(0x006181ccu) = 0xFFFFFFFFu;
    *reinterpret_cast<std::uint32_t*>(0x006181d0u) = 0xFFFFFFFFu;
    *reinterpret_cast<std::uint32_t*>(0x006181d4u) = 0xFFFFFFFFu;
    *reinterpret_cast<std::uint32_t*>(0x006181d8u) = 0xFFFFFFFFu;

    // Array cache: 4 entries x stride 0x10 at 0x007d40c0; total 0x40 bytes. [0x004c8650 body]
    const std::uintptr_t array_base = 0x007d40c0u;
    for (std::uint32_t uVar1 = 0u; uVar1 < 0x40u; uVar1 += 0x10u) {
        *reinterpret_cast<std::uint32_t*>(array_base + uVar1 + 0x00u) = 0xFFFFFFFFu;
        *reinterpret_cast<std::uint32_t*>(array_base + uVar1 + 0x04u) = 0x00000000u;
        *reinterpret_cast<std::uint32_t*>(array_base + uVar1 + 0x08u) = 0x00000000u;
    }
}

RH_ScopedInstall(rwD3D9RenderStateCacheInvalidate, 0x004c8650);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// rwD3D9CheckMMX  --  0x004dcf90
//
// Original: FUN_004dcf90 (87 bytes, 0x004dcf90..0x004dcfe6)
// Signature: uint FUN_004dcf90(void)
//   Returns: bit 23 of CPUID leaf-1 EDX (0 if CPUID unsupported)
//
// CPUID support detection + bit-23 query (MMX flag).
//
// Algorithm (cited from 0x004dcf90 body):
//   1. EFLAGS bit-21 toggle test (CPUID ID flag):
//      a. Read EFLAGS; OR with 4 (-> uVar2).
//      b. Flip bit 21: uVar3 = uVar2 ^ 0x200000.
//      c. Push uVar3 to EFLAGS, pop back, check if bit 21 survived.
//      d. If unchanged -> CPUID not supported -> return 0.
//   2. If supported:
//      cpuid_Version_info(1) — CPUID leaf 1.
//      return (result[+8] >> 0x17) & 1  — bit 23 of EDX = MMX flag.
//
// Constants (cited from 0x004dcf90 body):
//   0x200000 (2097152) — EFLAGS bit-21 (ID flag) mask
//   0x17 (23)          — bit shift for EDX[23] = MMX
//   1                  — leaf 1 for cpuid_Version_info
//
// Anti-island: callee=cpuid_Version_info (Ghidra intrinsic, C2+). Pure leaf otherwise.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004dcf90.md
// ---------------------------------------------------------------------------

// CPUID support check: returns true if the CPUID instruction is available.
// Replicates the EFLAGS bit-21 toggle test from the original. [0x004dcf90 body]
// Algorithm (from analysis): read EFLAGS, OR with 4, flip bit 21 (0x200000),
// push to EFLAGS, read back. If bit 21 can be changed → CPUID supported.
// Implementation: write bit-21 SET; read back; if bit 21 survived → supported.
static bool s_cpuidSupported()
{
    // Standard CPUID detection via EFLAGS bit-21 (ID flag) toggle. [0x004dcf90 body]
    // Read original EFLAGS, create version with bit-21 SET, write to EFLAGS,
    // read back. If bit-21 is still set → CPU allows EFLAGS.ID modification → CPUID ok.
    unsigned long eflags_orig, eflags_readback;
    __asm {
        pushfd
        pop   eax
        mov   eflags_orig, eax
        or    eax, 0x200000     // set bit 21
        push  eax
        popfd                   // write modified EFLAGS
        pushfd
        pop   eax               // read back
        mov   eflags_readback, eax
        // Restore original EFLAGS
        push  eflags_orig
        popfd
    }
    // If bit-21 survived, CPUID is supported. [0x004dcf90 body]
    return (eflags_readback & 0x200000u) != 0u;
}

// 0x004dcf90
extern "C" __declspec(dllexport) std::uint32_t __cdecl rwD3D9CheckMMX()
{
    // Step 1: CPUID support check. [0x004dcf90 body — EFLAGS bit-21 toggle]
    if (!s_cpuidSupported()) {
        return 0u;
    }

    // Step 2: CPUID leaf 1; extract EDX bit 23 (MMX). [0x004dcf90 body]
    int info[4];
    __cpuid(info, 1);
    // info[3] = EDX. Bit 23 = MMX feature flag.
    // Original: result[+8] >> 0x17 & 1  (result[+8] is EDX, i.e. info[3])
    return (static_cast<std::uint32_t>(info[3]) >> 0x17u) & 1u;
}

RH_ScopedInstall(rwD3D9CheckMMX, 0x004dcf90);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// rwD3D9CheckSSE_SSE2  --  0x004dcff0
//
// Original: FUN_004dcff0 (91 bytes, 0x004dcff0..0x004dd04b)
// Signature: bool FUN_004dcff0(void)
//   Returns: true if CPUID supported AND (EDX[25] OR EDX[26]) != 0
//
// CPUID support detection + bits-25/26 query (SSE / SSE2 combined check).
//
// Algorithm (cited from 0x004dcff0 body):
//   1. Identical EFLAGS bit-21 toggle boilerplate as FUN_004dcf90.
//      If CPUID not supported -> return 0.
//   2. If supported:
//      cpuid_Version_info(1) — CPUID leaf 1.
//      return (result[+8] & 0x6000000) != 0  — tests bits 25 AND 26 together.
//        EDX[25] = SSE, EDX[26] = SSE2.
//      Returns true if either SSE or SSE2 is present.
//
// Constants (cited from 0x004dcff0 body):
//   0x200000   — EFLAGS bit-21 mask
//   0x6000000  — EDX mask for bits 25+26 (0x02000000 | 0x04000000)
//
// Anti-island: callee=cpuid_Version_info (intrinsic). Pure leaf otherwise.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004dcff0.md
// ---------------------------------------------------------------------------

// 0x004dcff0
extern "C" __declspec(dllexport) std::uint32_t __cdecl rwD3D9CheckSSE_SSE2()
{
    // Step 1: CPUID support check (same boilerplate as rwD3D9CheckMMX). [0x004dcff0 body]
    if (!s_cpuidSupported()) {
        return 0u;
    }

    // Step 2: CPUID leaf 1; test bits 25+26 of EDX (SSE / SSE2). [0x004dcff0 body]
    int info[4];
    __cpuid(info, 1);
    // Original: (result[+8] & 0x6000000) != 0
    return ((static_cast<std::uint32_t>(info[3]) & 0x06000000u) != 0u) ? 1u : 0u;
}

RH_ScopedInstall(rwD3D9CheckSSE_SSE2, 0x004dcff0);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// rwD3D9CheckSSE2  --  0x004dd050
//
// Original: FUN_004dd050 (86 bytes, 0x004dd050..0x004dd0a6)
// Signature: uint FUN_004dd050(void)
//   Returns: bit 26 of CPUID leaf-1 EDX (0 if CPUID unsupported)
//
// CPUID support detection + bit-26 query (SSE2 only).
//
// Algorithm (cited from 0x004dd050 body):
//   Identical EFLAGS bit-21 toggle boilerplate as FUN_004dcf90 / FUN_004dcff0.
//   If supported:
//     cpuid_Version_info(1) — CPUID leaf 1.
//     return result[+8] >> 0x1a & 1  — bit 26 of EDX = SSE2 feature flag.
//
// Distinction from rwD3D9CheckSSE_SSE2: returns only the SSE2 bit in isolation
// vs. the OR of SSE + SSE2 (bits 25+26 vs. bit 26 alone).
//
// Constants (cited from 0x004dd050 body):
//   0x200000 — EFLAGS bit-21 mask
//   0x1a (26) — bit shift for EDX[26] = SSE2
//
// Anti-island: callee=cpuid_Version_info (intrinsic). Pure leaf otherwise.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004dd050.md
// ---------------------------------------------------------------------------

// 0x004dd050
extern "C" __declspec(dllexport) std::uint32_t __cdecl rwD3D9CheckSSE2()
{
    // Step 1: CPUID support check. [0x004dd050 body]
    if (!s_cpuidSupported()) {
        return 0u;
    }

    // Step 2: CPUID leaf 1; extract EDX bit 26 (SSE2). [0x004dd050 body]
    int info[4];
    __cpuid(info, 1);
    // Original: result[+8] >> 0x1a & 1  (bit 26 of EDX)
    return (static_cast<std::uint32_t>(info[3]) >> 0x1au) & 1u;
}

RH_ScopedInstall(rwD3D9CheckSSE2, 0x004dd050);  // re-enabled 2026-05-24 c3-render-b
