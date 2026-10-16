// Mashed RE - Race-results slot-state accessors.
// Analysis notes:
//   re/analysis/race_results/0040e470.md
//   re/analysis/c0_promotion_frontend_a/0x0040e480.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// INCLUDED (C2 leaf getters, no callees):
//   0x0040e470  CarSlotStateGet — getter: *(PTR_PTR_005f2770 + param_1*4 + 0x34)
//
// MOVED OUT 2026-10-16 (fwd494fd0, dual-copy burn-down): FUN_0040e480 CarSlotStateSet, the setter
// *(PTR_PTR_005f2770 + param_1*4 + 0x34) = param_2, is now ONE body shared by both targets in
// Ai/CarSlotState.cpp. This TU is .asi-only and the exe ran a private copy in AiStandalone.cpp.
//
// NOT INCLUDED (caller-gate or sig failures):
//   0x0040b460  SlotSortByScoreWithModeOverride — REFUSED: callee 0x00417740 not C2
//   0x0040e3a0  PlayerColorTableGet — already in MenuScoreSort.cpp; sig unsupported for diff

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// PTR_PTR_005f2770 — double-deref base for car-slot state table.
// Reads a pointer from address 0x005f2770, then indexes the pointed-to
// array by (param_1 * 4 + 0x34).
// Both constants cited from function body at 0x0040e470 (getter) and
// at 0x0040e480 (setter).
// ---------------------------------------------------------------------------

static inline std::uint32_t* SlotStateAddr(int param_1) {
    // 0x005f2770: base pointer-to-pointer (cited at 0x0040e470 body)
    std::uint32_t* base = *reinterpret_cast<std::uint32_t**>(0x005f2770u);
    // 0x34 = 52: fixed offset after param_1*4 stride (cited at 0x0040e470 body)
    return reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(base) + static_cast<unsigned>(param_1) * 4u + 0x34u);
}

// ---------------------------------------------------------------------------
// CarSlotStateGet  --  0x0040e470
//
// Original: FUN_0040e470 (14 bytes, 0x0040e470..0x0040e47e)
// Signature: undefined4 FUN_0040e470(int param_1)
//   param_1: car slot index (0-3)
// Returns *(PTR_PTR_005f2770 + param_1*4 + 0x34).
// No branches, no side-effects.
//
// Uncertainties:
//   U-1300: value semantics (what value 2 means) — does not affect reimpl.
//
// ref: re/analysis/race_results/0040e470.md
// ---------------------------------------------------------------------------

// 0x0040e470
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarSlotStateGet(int param_1) {
    // Return value at PTR_PTR_005f2770[param_1 * 4 + 0x34].
    // 0x005f2770 base cited at 0x0040e470 body; 0x34 offset cited at 0x0040e470 body.
    return *SlotStateAddr(param_1);
}

RH_ScopedInstall(CarSlotStateGet, 0x0040e470);  // re-enabled 2026-05-24 c3-render-a
