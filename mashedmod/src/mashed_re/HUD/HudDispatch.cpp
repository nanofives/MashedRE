// Mashed RE - HUD dispatch sub-mode accessor.
// Analysis note: re/analysis/hud_ingame_promote_c2/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// HudSubModeGet  --  0x0042f6a0
//
// Original: FUN_0042f6a0 (5 bytes, 0x0042f6a0..0x0042f6a5)
// Returns: DAT_0067e9fc (uint32_t global, 4 bytes).
// No callees, no branches, no side-effects.
// Used as the switch discriminant in FUN_0040dfc0 (per-frame HUD dispatch).
// Observed case values: 2, 3, 4, 5, 6, 10, 11 (0xb).
// ref: re/analysis/hud_ingame_promote_c2/0x0042f6a0.md
// ---------------------------------------------------------------------------

// 0x0042f6a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudSubModeGet() {
    // 0x0067e9fc: game sub-mode global; primary HUD dispatch discriminant
    return *reinterpret_cast<const std::uint32_t*>(0x0067e9fcu);
}

RH_ScopedInstall(HudSubModeGet, 0x0042f6a0);
