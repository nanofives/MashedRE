// Mashed RE - Frontend mode-index mapping.
// Analysis note: re/analysis/hud_frontend_d5/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "RowMovers.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FrontendModeIndex  --  0x004309b0
//
// Original: FUN_004309b0 (52 bytes, 0x004309b0..0x004309e4)
// Signature: int()
// Reads DAT_0067e9fc (game sub-mode global); computes iVar1 = value - 2;
// switch on iVar1 (cases 0..8); returns mapped index.
//
// Mode index table (from analysis note, 0x004309b5):
//   iVar1 == 0  (mode 2):    return 0
//   iVar1 == 1  (mode 3):    return 1
//   iVar1 == 2  (mode 4):    return 2
//   iVar1 == 3  (mode 5):    return 5
//   iVar1 == 4..7 (mode 6..9): return 3
//   iVar1 == 8  (mode 10):   return 11 (0xb)
//   default:                 return iVar1 (= DAT_0067e9fc - 2)
//
// No callees, no side-effects, no branches beyond the switch.
// ref: re/analysis/hud_frontend_d5/0x004309b0.md
// ---------------------------------------------------------------------------

// 0x004309b0 — the body, with the mode global as a parameter (RowMovers.h);
// the standalone's challenge launch gate calls it on its own game mode.
int mashed_re::Frontend::FrontendModeIndexFor(std::int32_t mode) {
    // 0x004309b5: switch on (mode - 2)
    const std::int32_t iVar1 = mode - 2;
    switch (iVar1) {
        case 0: return 0;    // mode 2 → index 0
        case 1: return 1;    // mode 3 → index 1
        case 2: return 2;    // mode 4 → index 2
        case 3: return 5;    // mode 5 → index 5
        case 4:              // mode 6 → index 3
        case 5:              // mode 7 → index 3
        case 6:              // mode 8 → index 3
        case 7: return 3;    // mode 9 → index 3
        case 8: return 0xb;  // mode 10 → index 11 (0xb)
        default: return iVar1; // other → mode - 2
    }
}

// 0x004309b0 — hook binding: reads DAT_0067e9fc (game mode global).
extern "C" __declspec(dllexport) int __cdecl FrontendModeIndex() {
    return mashed_re::Frontend::FrontendModeIndexFor(
        *reinterpret_cast<const std::int32_t*>(0x0067e9fcu));
}

RH_ScopedInstall(FrontendModeIndex, 0x004309b0);  // re-enabled 2026-05-24 c3-frontend-b
