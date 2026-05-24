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
//   0x0040e480  CarSlotStateSet — setter: *(PTR_PTR_005f2770 + param_1*4 + 0x34) = param_2
//
// NOT INCLUDED (caller-gate or sig failures):
//   0x0040b460  SlotSortByScoreWithModeOverride — REFUSED: callee 0x00417740 not C2
//   0x0040e3a0  PlayerColorTableGet — already in MenuScoreSort.cpp; sig unsupported for diff
//   0x0040e480  CarSlotStateSet — DEFERRED: no viable non-destructive diff arg_type
//                 for double-deref setter (entity_field_set readback wrong for PTR_PTR)
//                 D-row filed; promote after diff_template gains ptr_ptr_entity_set arg_type.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// PTR_PTR_005f2770 — double-deref base for car-slot state table.
// Reads a pointer from address 0x005f2770, then indexes the pointed-to
// array by (param_1 * 4 + 0x34).
// Both constants cited from function body at 0x0040e470 (getter) and
// 0x0040e480 (setter).
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

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(CarSlotStateGet, 0x0040e470);

// ---------------------------------------------------------------------------
// CarSlotStateSet  --  0x0040e480
//
// Original: FUN_0040e480 (18 bytes, 0x0040e480..0x0040e492)
// Signature: void FUN_0040e480(int param_1, undefined4 param_2)
//   param_1: car slot index (0-3)
//   param_2: value to write
// Writes param_2 to *(PTR_PTR_005f2770 + param_1*4 + 0x34).
// No branches, no callees.
//
// Uncertainties:
//   U-3431: struct type at 0x005f2770 — does not affect reimpl.
//
// DEFERRED from Frida diff: no non-destructive arg_type in diff_template.js
// for double-deref setter. entity_field_set reads target_global+p1*stride
// (single deref), which is incorrect here (double-deref via PTR_PTR_005f2770).
// Re-pickup: when diff_template.js gains a ptr_ptr_entity_set arg_type.
//
// ref: re/analysis/c0_promotion_frontend_a/0x0040e480.md
// ---------------------------------------------------------------------------

// 0x0040e480
extern "C" __declspec(dllexport) void __cdecl CarSlotStateSet(int param_1, std::uint32_t param_2) {
    // Write param_2 to PTR_PTR_005f2770[param_1 * 4 + 0x34].
    // 0x005f2770 base cited at 0x0040e480 body; 0x34 offset cited at 0x0040e480 body.
    *SlotStateAddr(param_1) = param_2;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(CarSlotStateSet, 0x0040e480);
