// Mashed RE — Boot/LaunchHandshake.cpp
// Launch-handshake thunks + engine-stop dispatcher.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Session: c3/batch-r-s1  (2026-05-22)
// No-guessing attestation: every RVA, constant, and callee address below
// is copied verbatim from the analysis notes which cite Ghidra decomp output.
// No semantic inference beyond what the decomp shows.
//
// Covers:
//   0x00493540  thunk_FUN_00495150  — 4-byte JMP to FUN_00495150 (lang/codeset gate, bool)
//   0x00493550  thunk_FUN_004938c0  — 4-byte JMP to FUN_004938c0 (engine-stop dispatch, void)
//   0x00493560  thunk_FUN_004954f0  — 4-byte JMP to FUN_004954f0 (HardwareExit + ShowCursor, returns 0)
//   0x004938c0  sub_004938c0        — engine-stop helper (5 sequential void calls, void)
//
// Analysis refs:
//   re/analysis/promote_c2_launch_handshake/00493540.md
//   re/analysis/promote_c2_launch_handshake/00493550.md
//   re/analysis/promote_c2_launch_handshake/00493560.md
//   re/analysis/promote_c2_launch_handshake/004938c0.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// External callees (all confirmed C2+ in hooks.csv):
//
//   0x00495150  FUN_00495150  — lang/codeset gate; returns bool   (C2)
//   0x004938c0  FUN_004938c0  — engine-stop helper; void          (C2, = EngineStopHelper below)
//   0x004954f0  FUN_004954f0  — HardwareExit gate; returns uint32 (C2)
//
//   Called by EngineStopHelper (0x004938c0):
//   0x00558470  FUN_00558470  — teardown void call #1  (C2)
//   0x00550390  FUN_00550390  — teardown void call #2  (C2)
//   0x004c2f60  FUN_004c2f60  — teardown void call #3  (C2)
//   0x004c3040  sub_004c3040  — teardown void call #4  (C2)
//   0x004c3270  sub_004c3270  — teardown void call #5  (C2)
// ---------------------------------------------------------------------------

using VoidFn_t   = void (__cdecl*)();
using BoolFn_t   = std::uint32_t (__cdecl*)();
using Uint32Fn_t = std::uint32_t (__cdecl*)();

// Thunk targets — called via original addresses (hook not installed on targets).
static BoolFn_t   const s_FUN_00495150 = reinterpret_cast<BoolFn_t>(0x00495150);
static VoidFn_t   const s_FUN_004938c0 = reinterpret_cast<VoidFn_t>(0x004938c0);
static Uint32Fn_t const s_FUN_004954f0 = reinterpret_cast<Uint32Fn_t>(0x004954f0);

// EngineStopHelper direct callees (used in sub_004938c0 reimpl).
static VoidFn_t const s_FUN_00558470 = reinterpret_cast<VoidFn_t>(0x00558470);
static VoidFn_t const s_FUN_00550390 = reinterpret_cast<VoidFn_t>(0x00550390);
static VoidFn_t const s_FUN_004c2f60 = reinterpret_cast<VoidFn_t>(0x004c2f60);
static VoidFn_t const s_FUN_004c3040 = reinterpret_cast<VoidFn_t>(0x004c3040);
static VoidFn_t const s_FUN_004c3270 = reinterpret_cast<VoidFn_t>(0x004c3270);


// ─── 0x00493540  thunk_FUN_00495150 ────────────────────────────────────────
//
// 4-byte thunk (E9 rel32) at 0x00493540..0x00493543; body_end = 0x00493544.
// Ghidra thunk=true; callees_depth1 = [] (JMP only — no local call instructions).
// Unconditionally transfers to FUN_00495150 at 0x00495150.
//
// Inlined target summary (from decomp via Ghidra):
//   Calls FUN_00495120, FUN_004955b0 (no args each).
//   Calls FUN_00499710 x2; passes results to FUN_004960a0, FUN_004963b0.
//   Reads DAT_007719e8 (0x007719e8) as switch discriminant:
//     case 1->2, 2->3, 3->4, 4->5, 5->1, default->0.
//   Calls FUN_00498510 with args (DAT_006147c0, uVar1).
//   Returns (DAT_007719e4 == 0).
//
// Anti-island: callee FUN_00495150 at C2; callers via WinMain chain.
// ref: re/analysis/promote_c2_launch_handshake/00493540.md

// 0x00493540
extern "C" __declspec(dllexport) std::uint32_t __cdecl thunk_LaunchLangGate() {
    return s_FUN_00495150();
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(thunk_LaunchLangGate, 0x00493540);


// ─── 0x00493550  thunk_FUN_004938c0 ────────────────────────────────────────
//
// 4-byte thunk (E9 rel32) at 0x00493550..0x00493553; body_end = 0x00493554.
// Ghidra thunk=true; callees_depth1 = [] (JMP only).
// Unconditionally transfers to FUN_004938c0 at 0x004938c0.
//
// Inlined target summary (from decomp via Ghidra):
//   Calls FUN_00558470, FUN_00550390, FUN_004c2f60, FUN_004c3040, FUN_004c3270
//   in sequence (no args each). No branches. Returns void.
//
// Anti-island: callee FUN_004938c0 at C2; callers via WinMain chain.
// ref: re/analysis/promote_c2_launch_handshake/00493550.md

// 0x00493550
extern "C" __declspec(dllexport) void __cdecl thunk_EngineStopDispatch() {
    s_FUN_004938c0();
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(thunk_EngineStopDispatch, 0x00493550);


// ─── 0x00493560  thunk_FUN_004954f0 ────────────────────────────────────────
//
// 4-byte thunk (E9 rel32) at 0x00493560..0x00493563; body_end = 0x00493564.
// Ghidra thunk=true; callees_depth1 = [] (JMP only).
// Unconditionally transfers to FUN_004954f0 at 0x004954f0.
//
// Inlined target summary (from decomp via Ghidra):
//   if (FUN_00498bf0() != 0): calls ShowCursor(1) [User32].
//   Calls FUN_00498b60, FUN_0045b350, thunk_FUN_00496370 (via 0x004963d0),
//         FUN_00496010, thunk_FUN_00495580 (via 0x004955c0). All no-args.
//   Returns literal 0 (undefined4).
//
// Constants: ShowCursor arg BOOL bShow = 0x1 (1, TRUE); return = 0x0.
//
// Anti-island: callee FUN_004954f0 at C2; callers via WinMain chain.
// ref: re/analysis/promote_c2_launch_handshake/00493560.md

// 0x00493560
extern "C" __declspec(dllexport) std::uint32_t __cdecl thunk_HwExitDispatch() {
    return s_FUN_004954f0();
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(thunk_HwExitDispatch, 0x00493560);


// ─── 0x004938c0  sub_004938c0  (EngineStopHelper) ──────────────────────────
//
// 24-byte function body (0x004938c0..0x004938d7).
// Takes no parameters; returns void.
//
// Decomp (verbatim from Ghidra, no-guessing):
//   FUN_00558470();   // teardown step 1 at 0x00558470
//   FUN_00550390();   // teardown step 2 at 0x00550390
//   FUN_004c2f60();   // teardown step 3 at 0x004c2f60
//   FUN_004c3040();   // teardown step 4 at 0x004c3040
//   FUN_004c3270();   // teardown step 5 at 0x004c3270
//   return;
//
// No branches, no constants, no global reads or writes in this function body.
// Callee semantics are deferred per U-3860 (callee purposes unknown at C2 level).
//
// Anti-island: all 5 callees at C2 (hooks.csv confirmed);
//   callers: FUN_00492370 and thunk_FUN_004938c0 (0x00493550, also this cluster).
// ref: re/analysis/promote_c2_launch_handshake/004938c0.md

// 0x004938c0
extern "C" __declspec(dllexport) void __cdecl EngineStopHelper() {
    s_FUN_00558470();
    s_FUN_00550390();
    s_FUN_004c2f60();
    s_FUN_004c3040();
    s_FUN_004c3270();
}

RH_ScopedInstall(EngineStopHelper, 0x004938c0);  // re-enabled 2026-05-24 batch-mixed
