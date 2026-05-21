// Mashed RE — Frontend small-leaf reimplementations (c3-batch-n session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0045ba00  RaceResultIndexedStore   — 15B leaf; indexed store to DAT_0068d1f0
//   0x0046c5c0  VehicleSlotInit          — 47B leaf; slot-init: clear+copy DAT_007f1030
//   0x0046c790  VehicleSlotFieldSet      — 31B leaf; writes field +0x0c in per-slot struct
//   0x004307a0  ElapsedVsThresholdCheck  — 116B dispatcher; reads float table + 3 callees
//
// Analysis notes:
//   re/analysis/promote_c1_low_ab1/0x0045ba00.md
//   re/analysis/promote_c1_low_ab1/0x0046c5c0.md
//   re/analysis/promote_c1_low_ab1/0x0046c790.md
//   re/analysis/frontend_c0_promote/0x004307a0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (0x004307a0 only; others are leaves)
// ---------------------------------------------------------------------------

// 0x00429a70  LapFracGetBySlot  — C3 (MenuScoreGetters.cpp)
// float __cdecl LapFracGetBySlot(uint32_t param_1)
// Returns (float)DAT_0067d99c[param_1].
static auto* const s_LapFracGetBySlot =
    reinterpret_cast<float(__cdecl*)(int)>(0x00429a70);

// 0x00429a80  LapLapsGetBySlot  — C3 (MenuScoreGetters.cpp)
// uint32_t __cdecl LapLapsGetBySlot(uint32_t param_1)
// Returns DAT_0067d98c[param_1].
static auto* const s_LapLapsGetBySlot =
    reinterpret_cast<std::int32_t(__cdecl*)(int)>(0x00429a80);

// 0x00429a90  LapSecsGetBySlot  — C3 (MenuScoreGetters.cpp)
// uint32_t __cdecl LapSecsGetBySlot(int param_1)
// Returns DAT_0067d994[param_1].
static auto* const s_LapSecsGetBySlot =
    reinterpret_cast<std::int32_t(__cdecl*)(int)>(0x00429a90);

// ---------------------------------------------------------------------------
// RaceResultIndexedStore  --  0x0045ba00
//
// Original: FUN_0045ba00 (15 bytes, 0x0045ba00..0x0045ba0e)
// Signature: void FUN_0045ba00(int param_1, undefined4 param_2)
//   param_1: array index
//   param_2: value to write
// Returns: void
//
// Body (pure indexed store, no branches):
//   *(undefined4*)(&DAT_0068d1f0 + param_1 * 4) = param_2  [0x0045ba03]
//
// Constants (cited from 0x0045ba00 body):
//   0x0068d1f0 — base of the indexed array
//   4          — stride (4 bytes per param_1 index)
//
// Uncertainties (non-blocking):
//   U-2869: DAT_0068d1f0 array element semantics unknown.
//
// ref: re/analysis/promote_c1_low_ab1/0x0045ba00.md
// ---------------------------------------------------------------------------

// 0x0045ba00
extern "C" __declspec(dllexport) void __cdecl RaceResultIndexedStore(
    int param_1, std::uint32_t param_2)
{
    // Write param_2 to (&DAT_0068d1f0)[param_1].
    // Base 0x0068d1f0 cited at 0x0045ba03; stride 4 bytes.
    *reinterpret_cast<std::uint32_t*>(0x0068d1f0u + static_cast<unsigned>(param_1) * 4u) = param_2;
}

RH_ScopedInstall(RaceResultIndexedStore, 0x0045ba00);

// ---------------------------------------------------------------------------
// VehicleSlotInit  --  0x0046c5c0
//
// Original: FUN_0046c5c0 (47 bytes, 0x0046c5c0..0x0046c5ee)
// Signature: undefined4 FUN_0046c5c0(uint param_1)
//   param_1: slot index (0..15 valid; > 15 → return 0)
// Returns: 1 on success, 0 if out-of-bounds.
//
// Body:
//   if (param_1 > 0xf) return 0;                              [0x0046c5c4]
//   uVar1 = DAT_007f1030;                                     [0x0046c5cb]
//   *(byte*)(&DAT_008815a4 + param_1 * 0x341) = 0;            [0x0046c5d3]
//   *(undefined4*)(&DAT_008820b0 + param_1 * 0xd04) = uVar1;  [0x0046c5df]
//   return 1;                                                  [0x0046c5ec]
//
// Constants (cited from 0x0046c5c0 body):
//   0x0000000f (15) — upper-bound inclusive for param_1             [0x0046c5c4]
//   0x007f1030      — global read into uVar1                        [0x0046c5cb]
//   0x008815a4      — first array base; stride 0x341 (833)          [0x0046c5d3]
//   0x00000341 (833)— stride for DAT_008815a4                       [0x0046c5d3]
//   0x008820b0      — second array base; stride 0xd04 (3332)        [0x0046c5df]
//   0x00000d04 (3332)—stride for DAT_008820b0                       [0x0046c5df]
//   1               — return value on success                       [0x0046c5ec]
//   0               — return value on out-of-bounds                 [0x0046c5c4]
//
// Uncertainties (non-blocking):
//   U-2870: DAT_008815a4 struct layout (stride 0x341, 16 slots); field at +0x00 zeroed.
//   U-2871: DAT_007f1030 identity — value copied to second array.
//
// ref: re/analysis/promote_c1_low_ab1/0x0046c5c0.md
// ---------------------------------------------------------------------------

// 0x0046c5c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleSlotInit(
    std::uint32_t param_1)
{
    // Bounds check: param_1 > 0xf (15) → return 0. [0x0046c5c4]
    if (param_1 > 0xfu) {
        return 0u;
    }

    // Read global DAT_007f1030. [0x0046c5cb]
    std::uint32_t uVar1 = *reinterpret_cast<std::uint32_t*>(0x007f1030u);

    // Zero field at DAT_008815a4 + param_1 * 0x341 (first array, field +0x00). [0x0046c5d3]
    *reinterpret_cast<std::uint8_t*>(0x008815a4u + param_1 * 0x341u) = 0u;

    // Write uVar1 to DAT_008820b0 + param_1 * 0xd04 (second array). [0x0046c5df]
    *reinterpret_cast<std::uint32_t*>(0x008820b0u + param_1 * 0xd04u) = uVar1;

    return 1u; // success [0x0046c5ec]
}

RH_ScopedInstall(VehicleSlotInit, 0x0046c5c0);

// ---------------------------------------------------------------------------
// VehicleSlotFieldSet  --  0x0046c790
//
// Original: FUN_0046c790 (31 bytes, 0x0046c790..0x0046c7ae)
// Signature: undefined4 FUN_0046c790(uint param_1, undefined4 param_2)
//   param_1: slot index (0..15 valid; > 15 → return 0xffffffff)
//   param_2: value to write
// Returns: 0xffffffff (-1) on out-of-bounds; 0 on success.
//
// Body:
//   if (param_1 > 0xf) return 0xffffffff;                      [0x0046c794]
//   *(undefined4*)(&DAT_008815b0 + param_1 * 0x341) = param_2; [0x0046c79e]
//   return 0;                                                   [0x0046c7ac]
//
// Note: DAT_008815b0 = DAT_008815a4 + 0xc (field +0x0c within per-slot struct).
// Same 0x341 stride as VehicleSlotInit.
//
// Constants (cited from 0x0046c790 body):
//   0x0000000f (15)  — upper-bound inclusive                    [0x0046c794]
//   0x008815b0       — DAT_008815a4 + 0xc; field +0x0c         [0x0046c79e]
//   0x00000341 (833) — stride (same as VehicleSlotInit)         [0x0046c79e]
//   0x00000000       — return on success                        [0x0046c7ac]
//   0xffffffff (-1)  — return on bounds-failure                 [0x0046c794]
//
// Uncertainties (non-blocking):
//   U-2872: field at +0x0c within 0x341-byte per-slot struct; semantics unknown.
//
// Dependency order: VehicleSlotInit (0x0046c5c0) must be GREEN before this is
// promoted; it is a callee of the caller 0x00422fd0 (FrontendRaceResultsDispatch C3).
//
// ref: re/analysis/promote_c1_low_ab1/0x0046c790.md
// ---------------------------------------------------------------------------

// 0x0046c790
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleSlotFieldSet(
    std::uint32_t param_1, std::uint32_t param_2)
{
    // Bounds check: param_1 > 0xf → return -1. [0x0046c794]
    if (param_1 > 0xfu) {
        return 0xffffffffu;
    }

    // Write param_2 to field +0x0c of per-slot struct at DAT_008815b0. [0x0046c79e]
    // DAT_008815b0 = DAT_008815a4 + 0xc; stride 0x341 (833 bytes/slot).
    *reinterpret_cast<std::uint32_t*>(0x008815b0u + param_1 * 0x341u) = param_2;

    return 0u; // success [0x0046c7ac]
}

RH_ScopedInstall(VehicleSlotFieldSet, 0x0046c790);

// ---------------------------------------------------------------------------
// ElapsedVsThresholdCheck  --  0x004307a0
//
// Original: FUN_004307a0 (116 bytes, 0x004307a0..0x00430814)
// Signature: undefined4 FUN_004307a0(void) — no parameters
// Returns: 1 if elapsed time < threshold, 0 otherwise.
//
// Algorithm (cited from 0x004307a0 body):
//   1. iVar5 = DAT_0067f17c * 0xc — row offset into table at DAT_00614718. [0x004307aa]
//   2. fVar4 = *(float*)(&DAT_00614718 + iVar5)  — threshold field 0.       [0x004307b5]
//   3. fVar1 = *(float*)(&DAT_00614720 + iVar5)  — threshold field 1.       [0x004307c1]
//   4. fVar2 = *(float*)(&DAT_0061471c + iVar5)  — threshold field 2.       [0x004307c7]
//   5. fVar4 *= *(float*)DAT_005cc728             — scale field 0.           [0x004307cd]
//   6. iVar5 = LapLapsGetBySlot(0)               — elapsed laps.            [call 0x004307db]
//   7. iVar6 = LapSecsGetBySlot(0)               — elapsed seconds.         [call]
//   8. fVar7 = (float)LapFracGetBySlot(0)        — elapsed fractional.      [call]
//   9. fVar7 += (float)(iVar5 * 60 + iVar6)      — total elapsed seconds.   [0x004307db: *60]
//  10. fVar3 = fVar4 + fVar1 + fVar2             — threshold sum.
//  11. return (fVar7 < fVar3) ? 1 : 0.
//
// Constants (cited from 0x004307a0 body):
//   0x0067f17c — row index global (track/session index) [0x004307aa]
//   0xc (12)   — stride per row in table               [0x004307aa]
//   0x00614718 — threshold field 0 base                [0x004307b5]
//   0x00614720 — threshold field 1 base (+8)           [0x004307c1]
//   0x0061471c — threshold field 2 base (+4)           [0x004307c7]
//   0x005cc728 — scale factor float for field 0        [0x004307cd]
//   60 (0x3c)  — seconds-per-lap multiplier            [0x004307db]
//
// Callees (all C3):
//   0x00429a70  LapFracGetBySlot  C3
//   0x00429a80  LapLapsGetBySlot  C3
//   0x00429a90  LapSecsGetBySlot  C3
//
// Uncertainties (non-blocking):
//   U-3596: table at DAT_00614718 field semantics (time budget units) unknown.
//   U-3597: DAT_0067f17c as row index: track/selection interpretation not confirmed.
//
// ref: re/analysis/frontend_c0_promote/0x004307a0.md
// ---------------------------------------------------------------------------

// 0x004307a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ElapsedVsThresholdCheck()
{
    // Step 1: compute row byte offset. [0x004307aa]
    std::int32_t row_idx = *reinterpret_cast<std::int32_t*>(0x0067f17cu);
    std::int32_t offset  = row_idx * 0xc;

    // Step 2-4: read three float threshold fields from table at DAT_00614718. [0x004307b5/c1/c7]
    float fVar4 = *reinterpret_cast<float*>(0x00614718u + static_cast<unsigned>(offset));
    float fVar1 = *reinterpret_cast<float*>(0x00614720u + static_cast<unsigned>(offset));
    float fVar2 = *reinterpret_cast<float*>(0x0061471cu + static_cast<unsigned>(offset));

    // Step 5: scale field 0 by *(float*)0x005cc728. [0x004307cd]
    float scale = *reinterpret_cast<float*>(0x005cc728u);
    fVar4 = fVar4 * scale;

    // Step 6-8: read elapsed time components via C3 callees. [calls near 0x004307db]
    std::int32_t iVar5 = s_LapLapsGetBySlot(0);    // elapsed laps
    std::int32_t iVar6 = s_LapSecsGetBySlot(0);    // elapsed seconds
    float        fVar7 = s_LapFracGetBySlot(0);    // elapsed fractional

    // Step 9: accumulate total elapsed. [0x004307db: *60 multiplier]
    fVar7 += static_cast<float>(iVar5 * 60 + iVar6);

    // Step 10-11: compare elapsed vs threshold sum. [return 1 if fVar7 < fVar3]
    float fVar3 = fVar4 + fVar1 + fVar2;
    return (fVar7 < fVar3) ? 1u : 0u;
}

RH_ScopedInstall(ElapsedVsThresholdCheck, 0x004307a0);
