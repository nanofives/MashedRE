// Mashed RE — promote-round round 1 (L0: c3_batch_race1 session-1 set).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   [MOVED 2026-11-01, D4-DC-INC-ai] AiVehicleFieldPtrGet (FUN_00408af0) and AiVehicleFloat4Get
//   (FUN_00442cc0) -> Ai/AiVehicleFieldGetters.cpp, one body each for both targets
//   0x00414030  AiSplineBankTimerReset  — ai;  spline-bank timer reset (writes 1000)
//   0x0040e350  GetRenderSubMode        — ai (used by vehicle/render too); global getter
//   0x00429300  HudOverlayFloatGet      — hud; float getter (kept here as the
//                                         round's cluster file; subsystem stays hud)
//
// Verification lane: scenario:'race' diffs (re/analysis/scenario_attach_lane.md).
//
// Analysis:
//   re/analysis/bucket_ai_00407a40_00415880/0x00408af0.md
//   re/analysis/bucket_ai_00415d00_00452ea0/0x00442cc0.md
//   re/analysis/bucket_ai_00407a40_00415880/0x00414030.md
//   re/analysis/promote_c2_vehicle_lowrva/0x0040e350.md
//   re/analysis/localization_d2/0x00429300.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// AiSplineBankTimerReset  --  0x00414030
//
// Original: FUN_00414030 (47 bytes, 0x00414030..0x0041405f)
// Signature: void FUN_00414030(int param_1)
//   param_1: vehicle slot index, or -1 for "all"
// Body:
//   if (param_1 == -1) {
//     for (puVar1 = &DAT_008032d4; (int)puVar1 < 0x803324; puVar1 += 5)
//       *puVar1 = 1000;
//     return;
//   }
//   (&DAT_008032d4)[param_1 * 5] = 1000;
//
// Constants (cited from function body at 0x00414030):
//   0x008032d4 — timer table base; per-slot stride 5 DWORDs (0x14 bytes)
//   0x00803324 — "all" loop upper bound (exclusive); exactly 5 slots
//   0x3e8      — reset value 1000 written per slot
//
// Caller: FUN_00417180 (AI line-bank switches).
//
// Uncertainties (non-blocking):
//   U-7564: unit/role of the table value 1000 (data-semantic).
// ---------------------------------------------------------------------------

// 0x00414030
extern "C" __declspec(dllexport) void __cdecl AiSplineBankTimerReset(int param_1) {
    if (param_1 == -1) {
        // 0x008032d4 base, 0x00803324 bound, stride 5 dwords, value 1000
        // cited at 0x00414030 body.
        std::uint32_t* p = reinterpret_cast<std::uint32_t*>(0x008032d4u);
        while (reinterpret_cast<std::uintptr_t>(p) < 0x00803324u) {
            *p = 1000u;
            p += 5;
        }
        return;
    }
    // single-slot write cited at 0x00414030 body.
    reinterpret_cast<std::uint32_t*>(0x008032d4u)[param_1 * 5] = 1000u;
}

RH_ScopedInstall(AiSplineBankTimerReset, 0x00414030);

// GetRenderSubMode (FUN_0040e350): MATCH-TOPDOWN-T13 moved the body to Match/MatchS02_0040e350.cpp,
// which compiles the byte-matched source re/match/s02/0040e350.c (one body per RVA, both targets).

// MATCH-S06: the body of 0x00429300 (HudOverlayFloatGet) moved to Match/MatchS06_00429300.cpp,
// which compiles the byte-matched source re/match/s06/00429300.c.
