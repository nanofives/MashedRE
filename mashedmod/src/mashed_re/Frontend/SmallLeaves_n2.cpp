// Mashed RE — Frontend small-leaf reimplementations (c3-batch-n session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0045ba00  RaceResultIndexedStore   — 15B leaf; indexed store to DAT_0068d1f0
//   0x004307a0  ElapsedVsThresholdCheck  — 116B dispatcher; reads float table + 3 callees
//
// Analysis notes:
//   re/analysis/promote_c1_low_ab1/0x0045ba00.md
//   re/analysis/frontend_c0_promote/0x004307a0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (0x004307a0 only; others are leaves)
// ---------------------------------------------------------------------------

// Callees of 0x004307a0, all three byte-matched leaves (C4 by match; one body each in
// Match/MatchS06_00429a70.cpp, Match/MatchS06_00429a80.cpp, Match/MatchS06_00429a90.cpp):
//   LapFracGetBySlot (orig. 0x00429a70)  returns (float)DAT_0067d99c[param_1]
//   LapLapsGetBySlot (orig. 0x00429a80)  returns DAT_0067d98c[param_1]
//   LapSecsGetBySlot (orig. 0x00429a90)  returns DAT_0067d994[param_1]
// PORT-FE-01 (2026-11-08): the exe binds them BY NAME to those matched exports. It used to call the
// original addresses, which the standalone does not map (only the six RVA thunks of exe_main.cpp exist),
// so any exe call of ElapsedVsThresholdCheck faulted. The .asi keeps the original-address calls (the
// installed hooks redirect them there), so its diff behaviour is unchanged.
#ifdef MASHED_STANDALONE
extern "C" float __cdecl LapFracGetBySlot(int param_1);
extern "C" std::uint32_t __cdecl LapLapsGetBySlot(int param_1);
extern "C" std::uint32_t __cdecl LapSecsGetBySlot(int param_1);
static auto* const s_LapFracGetBySlot = &LapFracGetBySlot;
static auto* const s_LapLapsGetBySlot = &LapLapsGetBySlot;
static auto* const s_LapSecsGetBySlot = &LapSecsGetBySlot;
#else
static auto* const s_LapFracGetBySlot =
    reinterpret_cast<float(__cdecl*)(int)>(0x00429a70);
static auto* const s_LapLapsGetBySlot =
    reinterpret_cast<std::int32_t(__cdecl*)(int)>(0x00429a80);
static auto* const s_LapSecsGetBySlot =
    reinterpret_cast<std::int32_t(__cdecl*)(int)>(0x00429a90);
#endif

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

// MATCH-S10: the body of 0x0045ba00 (RaceResultIndexedStore) moved to Match/MatchS10_0045ba00.cpp,
// which compiles the byte-matched source re/match/s10/0045ba00.c.

// MATCH-FIX-46C5C0: the bodies of 0x0046c5c0 (VehicleSlotInit) and 0x0046c790 (VehicleSlotFieldSet) moved to
// Match/MatchS11_0046c5c0.cpp and Match/MatchS11_0046c790.cpp, which compile the byte-matched sources
// re/match/s11/0046c5c0.c and 0046c790.c. The old bodies here used stride 0x341 where the original uses 0xd04
// (0x0046c5c0 also stored a BYTE where the original stores a DWORD).

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

RH_ScopedInstall(ElapsedVsThresholdCheck, 0x004307a0);  // re-enabled 2026-05-24 c3-frontend-b
