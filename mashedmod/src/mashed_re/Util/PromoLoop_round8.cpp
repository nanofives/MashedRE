// Mashed RE — promote-round round 8 (L3 curated small leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004430a0  Util897fe0Set       — util; 9B setter
//   0x004430b0  Util897fe0Get       — util; 5B getter (same global)
//   0x0042f790  GhostModeIsActive   — util; 5B getter (hooks.csv GhostMode::IsActive)
//   0x00431d70  CourseGetLeaderIndex — util; 5B getter (hooks.csv Course::GetLeaderIndex)
//   0x004cc7e0  RwGlobal6182b0Set   — render; 9B setter
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
// All five run menu-attach with SEEDED handlers (read_global /
// void_setter_observe) — discriminating regardless of live state.
//
// Analysis:
//   re/analysis/game_state_d5/0x004430a0.md
//   re/analysis/game_state_d5/0x004430b0.md
//   re/analysis/leaderboard_d2/0x0042f790.md
//   re/analysis/leaderboard_d2/0x00431d70.md
//   re/analysis/render_5_c1_to_c2_s1/FUN_004cc7e0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Util897fe0Set  --  0x004430a0   (subsystem: util)
//
// Original: FUN_004430a0 (10 bytes, 0x004430a0..0x004430a9)
// Bytes: 8B 44 24 04 / A3 E0 7F 89 00 / C3
//   (mov eax,[esp+4]; mov [0x00897fe0],eax; ret)
// Signature: void FUN_004430a0(undefined4 param_1)
//
// Constants (cited from function body at 0x004430a0):
//   0x00897fe0 — destination global dword
//
// Callers: FUN_00445aa0 (C2, calls with 0 in the player-slot loop),
//          FUN_004102f0 (C2, calls with 1). Paired getter: 0x004430b0.
// No uncertainties (body fully legible per the note).
// ---------------------------------------------------------------------------

// 0x004430a0
extern "C" __declspec(dllexport) void __cdecl Util897fe0Set(std::uint32_t param_1) {
    // 0x00897fe0 cited at 0x004430a0 body.
    *reinterpret_cast<std::uint32_t*>(0x00897fe0u) = param_1;
}

RH_ScopedInstall(Util897fe0Set, 0x004430a0);

// ---------------------------------------------------------------------------
// Util897fe0Get  --  0x004430b0   (subsystem: util)
//
// Original: FUN_004430b0 (6 bytes, 0x004430b0..0x004430b5)
// Bytes: A1 E0 7F 89 00 / C3   (mov eax,[0x00897fe0]; ret)
// Signature: undefined4 FUN_004430b0(void)
//
// Getter counterpart of Util897fe0Set (same global).
//
// Constants (cited from function body at 0x004430b0):
//   0x00897fe0 — source global dword
//
// Caller: FUN_004102f0 (C2). No uncertainties.
// ---------------------------------------------------------------------------

// 0x004430b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Util897fe0Get(void) {
    // 0x00897fe0 cited at 0x004430b0 body.
    return *reinterpret_cast<const std::uint32_t*>(0x00897fe0u);
}

RH_ScopedInstall(Util897fe0Get, 0x004430b0);

// ---------------------------------------------------------------------------
// GhostModeIsActive  --  0x0042f790   (subsystem: util)
//
// Original: FUN_0042f790 / GhostMode::IsActive (6 bytes, 0x0042f790..0x0042f795)
// Bytes: A1 70 EA 67 00 / C3   (mov eax,[0x0067ea70]; ret)
// Signature: undefined4 FUN_0042f790(void)
//
// Constants (cited from function body at 0x0042f790, load at 0x0042f792):
//   0x0067ea70 — ghost mode flag (0 = normal, non-0 = ghost per the note)
//
// Caller: Course::Finish 0x0040d270 (C2). Resolves S-2225 (already resolved).
// ---------------------------------------------------------------------------

// 0x0042f790
extern "C" __declspec(dllexport) std::uint32_t __cdecl GhostModeIsActive(void) {
    // 0x0067ea70 cited at 0x0042f792.
    return *reinterpret_cast<const std::uint32_t*>(0x0067ea70u);
}

RH_ScopedInstall(GhostModeIsActive, 0x0042f790);

// ---------------------------------------------------------------------------
// CourseGetLeaderIndex  --  0x00431d70   (subsystem: util)
//
// Original: FUN_00431d70 / Course::GetLeaderIndex (6 bytes, 0x00431d70..0x00431d75)
// Bytes: A1 94 EA 67 00 / C3   (mov eax,[0x0067ea94]; ret)
// Signature: undefined4 FUN_00431d70(void)
//
// Constants (cited from function body at 0x00431d70, load at 0x00431d72):
//   0x0067ea94 — leader/target car index (modes 10 + 6 override per the note)
//
// Caller: Course::ValidateCarsFinished 0x0040d040 (C2).
// ---------------------------------------------------------------------------

// 0x00431d70
extern "C" __declspec(dllexport) std::uint32_t __cdecl CourseGetLeaderIndex(void) {
    // 0x0067ea94 cited at 0x00431d72.
    return *reinterpret_cast<const std::uint32_t*>(0x0067ea94u);
}

RH_ScopedInstall(CourseGetLeaderIndex, 0x00431d70);

// ---------------------------------------------------------------------------
// RwGlobal6182b0Set  --  0x004cc7e0   (subsystem: render)
//
// Original: FUN_004cc7e0 (10 bytes, 0x004cc7e0..0x004cc7e9)
// Bytes: 8B 44 24 04 / A3 B0 82 61 00 / C3
//   (mov eax,[esp+4]; mov [0x006182b0],eax; ret)
// Signature: void FUN_004cc7e0(undefined4 param_1)
//
// Constants (cited from function body, write target at 0x004cc7e2):
//   0x006182b0 — destination global dword (guard read by FUN_004cc820 per U-5102)
//
// Caller: FUN_004c32b0 (RW engine init orchestrator, C2).
//
// Uncertainties (non-blocking):
//   U-5102: semantic role of DAT_006182b0 (data-semantic).
// ---------------------------------------------------------------------------

// 0x004cc7e0
extern "C" __declspec(dllexport) void __cdecl RwGlobal6182b0Set(std::uint32_t param_1) {
    // 0x006182b0 cited at 0x004cc7e2.
    *reinterpret_cast<std::uint32_t*>(0x006182b0u) = param_1;
}

RH_ScopedInstall(RwGlobal6182b0Set, 0x004cc7e0);
