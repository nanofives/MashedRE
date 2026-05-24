// Mashed RE — Boot/GameStateCluster.cpp
// Skeleton game-state cluster: 6 pure-leaf getters/setters/zeroing ops.
//
// Covers:
//   0x00426c00  GameStateFlagGet      — returns DAT_00644158 (pure DWORD getter)
//   0x0042b8e0  StatePhaseIsTwo       — returns (DAT_0067eca4 == 2) as bool
//   0x0042b910  RaceEndConstGet       — returns literal constant 5
//   0x0042b940  StatePhaseSubSet      — writes param_1 to DAT_0067ecb0 (pure DWORD setter)
//   0x0042c1c0  RaceInterruptFlagGet  — returns DAT_0067eab0 (pure DWORD getter)
//   0x0042c1d0  RaceStateArrayZero    — zeroes 12 DWORDs at DAT_0067ea10
//
// All 6 are pure leaf functions (callees_depth1: []).
// Leaf-function exemption applies for C2->C3 structural anti-island rule.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/skeleton_prep_game_state/00426c00.md
//   re/analysis/skeleton_prep_game_state/0042b8e0.md
//   re/analysis/skeleton_prep_game_state/0042b910.md
//   re/analysis/skeleton_prep_game_state/0042b940.md
//   re/analysis/skeleton_prep_game_state/0042c1c0.md
//   re/analysis/skeleton_prep_game_state/0042c1d0.md

#include "../Core/HookSystem.h"
#include <cstdint>


// ---------------------------------------------------------------------------
// GameStateFlagGet  --  0x00426c00
//
// Original: FUN_00426c00 (5 bytes, 0x00426c00–0x00426c04).
// Decompiled body (verbatim from analysis note 00426c00.md):
//   MOV EAX, [0x00644158]
//   RET
//
// Pure global getter: returns DAT_00644158 without modification.
// No branches, no writes, no callees.
// Callers compare result against 0x21 (33 dec).
//
// Constants cited (from analysis table):
//   0x00426c01  0x00644158  — DWORD global read and returned
//
// Uncertainties:
//   [UNCERTAIN U-0421] Semantic meaning of DAT_00644158; callers compare to 0x21.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x00402a40.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x00426c00
extern "C" __declspec(dllexport) std::uint32_t __cdecl GameStateFlagGet() {
    // 0x00426c01: read and return DAT_00644158.
    return *reinterpret_cast<const std::uint32_t*>(0x00644158u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(GameStateFlagGet, 0x00426c00);


// ---------------------------------------------------------------------------
// StatePhaseIsTwo  --  0x0042b8e0
//
// Original: FUN_0042b8e0 (14 bytes, 0x0042b8e0–0x0042b8ed).
// Decompiled body (verbatim from analysis note 0042b8e0.md):
//   MOV EAX, [0x0067eca4]
//   CMP EAX, 2
//   SETE AL
//   MOVZX EAX, AL
//   RET
//
// Returns 1 if DAT_0067eca4 == 2, else 0.
// No writes, no callees. Paired with StatePhaseIsIdle (== 0) and StatePhaseIsFinal (== 5).
// STATE_FN case 5 uses this to gate state-phase transition.
//
// Constants cited (from analysis table):
//   0x0042b8e2  0x0067eca4  — state-phase DWORD global
//   comparand   0x00000002  — equality comparand (2 dec)
//
// Uncertainties:
//   [UNCERTAIN U-0500] Semantic meaning of DAT_0067eca4 == 2; which phase/state.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x004929d0.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x0042b8e0
extern "C" __declspec(dllexport) int __cdecl StatePhaseIsTwo() {
    // 0x0042b8e2: read DAT_0067eca4; return 1 if == 2, else 0.
    return (*reinterpret_cast<const std::uint32_t*>(0x0067eca4u) == 2u) ? 1 : 0;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(StatePhaseIsTwo, 0x0042b8e0);


// ---------------------------------------------------------------------------
// RaceEndConstGet  --  0x0042b910
//
// Original: FUN_0042b910 (5 bytes, 0x0042b910–0x0042b914).
// Decompiled body (verbatim from analysis note 0042b910.md):
//   MOV EAX, 5
//   RET
//
// Returns the hardcoded constant 5. No memory reads, no writes, no callees.
// Called from STATE_FN case 3 race-end path; return value passed to FUN_004331a0 (STUB S-0487).
//
// Constants cited (from analysis table):
//   0x0042b911  0x00000005  — hardcoded constant returned
//
// Uncertainties: none.
//
// Caller: 0x004929d0.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x0042b910
extern "C" __declspec(dllexport) int __cdecl RaceEndConstGet() {
    // 0x0042b911: return literal constant 5.
    return 5;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RaceEndConstGet, 0x0042b910);


// ---------------------------------------------------------------------------
// StatePhaseSubSet  --  0x0042b940
//
// Original: FUN_0042b940 (9 bytes, 0x0042b940–0x0042b948).
// Decompiled body (verbatim from analysis note 0042b940.md):
//   MOV EAX, [ESP+4]   ; param_1
//   MOV [0x0067ecb0], EAX
//   RET
//
// Writes param_1 unconditionally to DAT_0067ecb0. Returns void.
// No branches, no callees.
// Paired getter: FUN_0042b930 (0x0042b930) reads the same global.
//
// Constants cited (from analysis table):
//   0x0042b942  0x0067ecb0  — destination DWORD global written with param_1
//
// Uncertainties:
//   [UNCERTAIN U-0501] Semantic meaning of DAT_0067ecb0; which subsystem reads it.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x004929d0.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x0042b940
extern "C" __declspec(dllexport) void __cdecl StatePhaseSubSet(std::uint32_t param_1) {
    // 0x0042b942: DAT_0067ecb0 = param_1.
    *reinterpret_cast<std::uint32_t*>(0x0067ecb0u) = param_1;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(StatePhaseSubSet, 0x0042b940);


// ---------------------------------------------------------------------------
// RaceInterruptFlagGet  --  0x0042c1c0
//
// Original: FUN_0042c1c0 (5 bytes, 0x0042c1c0–0x0042c1c4).
// Decompiled body (verbatim from analysis note 0042c1c0.md):
//   MOV EAX, [0x0067eab0]
//   RET
//
// Pure global getter: returns DAT_0067eab0 without modification.
// No writes, no branches, no callees.
// STATE_FN case 3: if non-zero → sets DAT_00771968 = 7 (race-interruption path).
//
// Constants cited (from analysis table):
//   0x0042c1c1  0x0067eab0  — race-interruption flag DWORD global read and returned
//
// Uncertainties:
//   [UNCERTAIN U-0502] Semantic meaning of DAT_0067eab0; what events set it non-zero.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x004929d0.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x0042c1c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RaceInterruptFlagGet() {
    // 0x0042c1c1: read and return DAT_0067eab0.
    return *reinterpret_cast<const std::uint32_t*>(0x0067eab0u);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RaceInterruptFlagGet, 0x0042c1c0);


// ---------------------------------------------------------------------------
// RaceStateArrayZero  --  0x0042c1d0
//
// Original: FUN_0042c1d0 (16 bytes, 0x0042c1d0–0x0042c1df).
// Decompiled body (verbatim from analysis note 0042c1d0.md):
//   LEA EAX, [0x0067ea10]
//   MOV ECX, 0xC
//   zero-loop: DWORD PTR [EAX] = 0; ADD EAX, 4; LOOP
//
// Zeroes 12 consecutive DWORDs starting at DAT_0067ea10.
// Memory range zeroed: 0x0067ea10 through 0x0067ea3f (12 × 4 = 48 bytes).
// No callees. Called unconditionally from STATE_FN case 3.
//
// Constants cited (from analysis table):
//   0x0042c1d2  0x0067ea10  — base of 12-DWORD array to zero
//   loop count  0x0000000C  — 12 iterations (48 bytes total)
//   range end   0x0067ea3f  — last byte zeroed
//
// Uncertainties:
//   [UNCERTAIN U-0503] Semantic meaning of the 12-DWORD array at 0x0067ea10.
//                       Blocks=none (leaf exemption applies).
//
// Caller: 0x004929d0.
// Callees: none (leaf exemption applies).
// ---------------------------------------------------------------------------

// 0x0042c1d0
extern "C" __declspec(dllexport) void __cdecl RaceStateArrayZero() {
    // 0x0042c1d2: base address of 12-DWORD array.
    std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(0x0067ea10u);
    // loop count: 0x0C = 12 iterations.
    int iVar1 = 0xC;
    do {
        // zero-loop body: *puVar2 = 0; puVar2++.
        *puVar2 = 0u;
        ++puVar2;
        --iVar1;
    } while (iVar1 != 0);
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RaceStateArrayZero, 0x0042c1d0);
