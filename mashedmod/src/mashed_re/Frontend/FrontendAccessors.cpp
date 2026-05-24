// Mashed RE - Frontend accessor reimplementations.
// Analysis note: re/analysis/hud_frontend_d3/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FrontendGlobalGet  --  0x0040ad20
//
// Original: FUN_0040ad20 (6 bytes, 0x0040ad20..0x0040ad26)
// Returns: DAT_008a95ac (uint32_t global, 4 bytes).
// No parameters, no branches, no side-effects.
// ref: re/analysis/hud_frontend_d3/0x0040ad20.md
// ---------------------------------------------------------------------------

// 0x0040ad20
extern "C" __declspec(dllexport) std::uint32_t __cdecl FrontendGlobalGet() {
    // 0x008a95ac: global read address (cited at RVA 0x0040ad20)
    return *reinterpret_cast<const std::uint32_t*>(0x008a95acu);
}

RH_ScopedInstall(FrontendGlobalGet, 0x0040ad20);  // re-enabled 2026-05-24 (pre-regression C4 leaf)

// ---------------------------------------------------------------------------
// FrontendArrayGet  --  0x0040b6c0
//
// Original: FUN_0040b6c0 (11 bytes, 0x0040b6c0..0x0040b6cb)
// Signature: uint32_t(int param_1)
// Returns: DAT_008a94f0[param_1] — 4-byte indexed read from array base.
// No branches, no side-effects.
// Array base address cited at: 0x0040b6c3 -> 0x008a94f0
// Note: no bounds check in original — cast param_1 directly as index.
// ref: re/analysis/hud_frontend_d3/0x0040b6c0.md
// ---------------------------------------------------------------------------

// 0x0040b6c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl FrontendArrayGet(int param_1) {
    // 0x0040b6c3: indexed array base address 0x008a94f0
    const std::uint32_t* arr = reinterpret_cast<const std::uint32_t*>(0x008a94f0u);
    return arr[static_cast<std::uint32_t>(param_1)];
}

RH_ScopedInstall(FrontendArrayGet, 0x0040b6c0);  // re-enabled 2026-05-24 (pre-regression C4 leaf)
