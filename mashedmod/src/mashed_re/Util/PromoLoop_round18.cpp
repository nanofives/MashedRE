// Mashed RE — promote-round round 18 (widened curation: 16-60B getters).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x0041efc0  CarGetLapProgress — util; double-deref per-car field getter
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-12.
//
// DEFERRED this round (see PROMOTION_LOOP_LEDGER):
//   0x0046cbb0 — two-out-pointer (fn(i, out_state*, out_secondary*)); needs the
//                unbuilt out-buffer-compare handler (L5 wishlist).
//   0x0041c010 — 116B float-block writer; its callee FUN_0041b770 (C2) advances
//                an unobservable global cursor -> A/B state hazard for a
//                save/restore diff. Needs the cursor address pinned first.
//
// Analysis: re/analysis/leaderboard_d2/0x0041efc0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// CarGetLapProgress  --  0x0041efc0   (subsystem: util; hooks.csv Car::GetLapProgress)
//
// Original: FUN_0041efc0 (23 bytes, 0x0041efc0..0x0041efd6)
// Bytes: 8B 44 24 04 / 69 C0 AC 02 00 00 / 8B 88 48 DC 63 00 / 8B 81 80 00 00 00 / C3
//   (mov eax,[esp+4]; imul eax,eax,0x2ac;
//    mov ecx,[eax+0x0063dc48];   <- ptr = car_array[i].field0
//    mov eax,[ecx+0x80];         <- return ptr[+0x80]
//    ret)
// Signature: undefined4 FUN_0041efc0(int param_1)
//
// DOUBLE-DEREF: reads a pointer from the per-car struct at 0x0063dc48 + i*0x2ac,
// then returns the int at +0x80 of the pointed object. The car-data pointers
// are NULL until a race loads, so this MUST run on the race lane with in-bounds
// car indices (0..3 for a 4-car Quick Battle).
//
// Constants (cited from function body at 0x0041efc0):
//   0x0063dc48 — per-car struct array base (stride 0x2ac = 684 bytes)
//   0x2ac      — per-car stride
//   0x80       — lap/progress field offset within the pointed object
//
// Caller: FUN_0040d040 Course::ValidateCarsFinished (util, C2). Leaf (no calls).
// ---------------------------------------------------------------------------

// 0x0041efc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarGetLapProgress(int param_1) {
    // 0x0063dc48 base + 0x2ac stride cited at 0x0041efc0 body.
    const std::int32_t ptr = *reinterpret_cast<const std::int32_t*>(
        0x0063dc48u + static_cast<std::uint32_t>(param_1) * 0x2acu);
    // +0x80 field cited at 0x0041efc0 body.
    return *reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uint32_t>(ptr) + 0x80u);
}

RH_ScopedInstall(CarGetLapProgress, 0x0041efc0);
