// Mashed RE - Championship mode and game-mode car-slot state helpers.
// Analysis notes: re/analysis/race_results/0040e470.md
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// 0x0040e470  FUN_0040e470  CarSlotStateGet
//
// Pure 14-byte getter. Returns the DWORD at
//   *(*(uint32_t**)0x005f2770 + param_1*4 + 0x34)
//
// param_1  — car/player slot index (0–3).
// return   — raw slot-state DWORD (1=player-active, 2=AI/filtered; U-1300).
//
// No branches, no calls, no writes. Only constant cited:
//   PTR_PTR_005f2770 at 0x0040e470 body.
//   Stride 4, base offset 0x34 (52 dec) at 0x0040e470 body.
//
// Callers: FUN_0042b770 (C3), FUN_0042aff0 (C3), FUN_0042b180 (C3),
//          FUN_0042b310 (C3), FUN_0042ae10 (C2), FUN_0042c220 (C1).
// ref: re/analysis/race_results/0040e470.md
// ---------------------------------------------------------------------------

// 0x0040e470
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarSlotStateGet(int param_1) {
    const auto** ppBase = reinterpret_cast<const std::uint32_t**>(0x005f2770u);
    const auto*  pBase  = *ppBase;
    return *(pBase + param_1 + (0x34 / sizeof(std::uint32_t)));
}

RH_ScopedInstall(CarSlotStateGet, 0x0040e470);
