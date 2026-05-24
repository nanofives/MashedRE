// Mashed RE — Util leaf functions, c3-batch-k session 3.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file:
//   0x0040e340  GetLiveCarCount       — getter: DAT_008a94d0 (live-car count / dispatch key)
//   0x0040e370  IsCarSlotActive       — bool getter: is car-slot N active?
//
// NOT INCLUDED (deferred — callee_gate or signature_unsupported):
//   0x00412f30  FUN_00412f30          — REFUSED: callee_gate; 4 depth-1 callees at C1
//                                       (0x0046d4a0 C1, 0x00467210 C1, 0x0041f0d0 C1,
//                                        0x00412e30 C1). Re-pickup when those reach C2.
//                                       D-9340 already filed; no new DEFERRED row needed.
//   0x004997b0  FUN_004997b0          — REFUSED: signature_unsupported; 4-arg function
//                                       (ushort, LPCSTR, undefined4*, DWORD*) has no
//                                       matching arg_type in re/frida/diff_template.js.
//                                       Queue a harness extension before re-attempting.
//
// Analysis:
//   re/analysis/promote_c2_vehicle_lowrva/0x0040e340.md
//   re/analysis/promote_c2_vehicle_lowrva/0x0040e370.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// GetLiveCarCount  --  0x0040e340
//
// Original: FUN_0040e340 (5 bytes, 0x0040e340..0x0040e344)
// Signature: undefined4 FUN_0040e340(void)
// Body: MOV EAX, [0x008a94d0]  /  RETN
// Returns the dword at DAT_008a94d0.
//
// Constants (cited from function body at 0x0040e340):
//   0x008a94d0 — global dword; used by FUN_00410d10 case-7 loop as upper bound
//               for counting eliminated cars.
//
// Caller: FUN_00410d10 (vehicle DAMAGE_FN case 7); FUN_00412f30.
//
// Uncertainties (non-blocking):
//   U-0497: writer of DAT_008a94d0 not yet documented (likely race-start init).
//   subsystem_observed: util_or_vehicle (hooks.csv has util).
//
// ref: re/analysis/promote_c2_vehicle_lowrva/0x0040e340.md
// ---------------------------------------------------------------------------

// 0x0040e340
extern "C" __declspec(dllexport) std::uint32_t __cdecl GetLiveCarCount() {
    // Return DAT_008a94d0.
    // 0x008a94d0 cited at 0x0040e340 body.
    return *reinterpret_cast<const std::uint32_t*>(0x008a94d0u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(GetLiveCarCount, 0x0040e340);

// ---------------------------------------------------------------------------
// IsCarSlotActive  --  0x0040e370
//
// Original: FUN_0040e370 (33 bytes, 0x0040e370..0x0040e390)
// Signature: bool FUN_0040e370(int param_1)
//   param_1: car slot index
// Body:
//   if (3 < param_1) return false;
//   return *(int *)(PTR_PTR_005f2770 + param_1 * 4 + 0x34) != 0;
//
// Constants (cited from function body at 0x0040e370):
//   3    — upper-bound check; returns false for param_1 > 3.
//   0x34 — base offset into the per-car-slot table.
//   4    — slot stride (dword).
//   PTR_PTR_005f2770 — double-deref base; same table as FUN_0040e180 and
//                      FUN_00410d10.
//
// Return: true (1) if slot is non-null, false (0) otherwise or if out-of-bounds.
//
// Uncertainties (non-blocking):
//   U-1611: PTR_PTR_005f2770 owner not yet resolved (S-1840 covers this).
//   subsystem_observed: util_or_vehicle (hooks.csv has util).
//
// ref: re/analysis/promote_c2_vehicle_lowrva/0x0040e370.md
// ---------------------------------------------------------------------------

// 0x0040e370
extern "C" __declspec(dllexport) std::uint32_t __cdecl IsCarSlotActive(int param_1) {
    // Bounds check: param_1 must be in 0..3.
    // 3 cited at 0x0040e370 body.
    if (3 < param_1) {
        return 0u;
    }
    // Double-deref: read pointer from 0x005f2770, then index by param_1*4 + 0x34.
    // 0x005f2770 base cited at 0x0040e370 body.
    // 0x34 offset cited at 0x0040e370 body.
    // 4 stride cited at 0x0040e370 body.
    std::uint32_t* base = *reinterpret_cast<std::uint32_t**>(0x005f2770u);
    std::uint32_t val = *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(base) + static_cast<unsigned>(param_1) * 4u + 0x34u);
    return val != 0u ? 1u : 0u;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(IsCarSlotActive, 0x0040e370);
