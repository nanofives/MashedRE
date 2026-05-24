// Mashed RE — timer_d3_cont1_b trivial setter cluster.
// Four pure-leaf global-write functions from the timer/state subsystem.
//
// 0x0041d820  FUN_0041d820  timer_d3_cont1_b-20260512
//   Writes constant 0 to DAT_0063d558 (10 bytes: MOV DWORD PTR [imm32],0 + RET).
//   Only caller: FUN_004111c0 (large init function).
//   [UNCERTAIN] Role of DAT_0063d558 is semantic-only (see analysis note).
//
// 0x0041e130  FUN_0041e130  timer_d3_cont1_b-20260512
//   Writes param_1 to DAT_0063d7e0 (9 bytes: MOV [imm32],EAX + RET).
//   Callers: FUN_004111c0, FUN_0040e560.
//
// 0x00426630  FUN_00426630  timer_d3_cont1_b-20260512
//   Writes param_1 to DAT_0066d6fc (9 bytes: MOV [imm32],EAX + RET).
//   Only caller: FUN_004111c0. Sibling of FUN_004266f0 (target +4).
//
// 0x004266f0  FUN_004266f0  timer_d3_cont1_b-20260512
//   Writes param_1 to DAT_0066d700 (9 bytes: MOV [imm32],EAX + RET).
//   Only caller: FUN_004111c0. Sibling of FUN_00426630 (target -4).
#include "../Core/HookSystem.h"

#include <cstdint>

// ── 0x0041d820 ───────────────────────────────────────────────────────────────
// TimerFlagClear — writes 0 to DAT_0063d558.
// Body (10 bytes):
//   C7 05 58 D5 63 00 00 00 00 00  MOV DWORD PTR [0x0063d558], 0
//   C3                             RET
static constexpr std::uintptr_t kTimerFlag0063d558 = 0x0063d558;

extern "C" __declspec(dllexport) void __cdecl TimerFlagClear() {
    *reinterpret_cast<std::uint32_t*>(kTimerFlag0063d558) = 0u;
}

// RH_ScopedInstall(TimerFlagClear, 0x0041d820);  // first-wins; superseded
// (// frida-sweep first-wins (TimerInit.cpp/AudioRws.cpp registered first): 0x0041d820)

// ── 0x0041e130 ───────────────────────────────────────────────────────────────
// TimerStateSet — writes param_1 to DAT_0063d7e0.
// Body (9 bytes):
//   A3 E0 D7 63 00  MOV [0x0063d7e0], EAX
//   C3              RET
static constexpr std::uintptr_t kTimerState0063d7e0 = 0x0063d7e0;

extern "C" __declspec(dllexport) void __cdecl TimerStateSet(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(kTimerState0063d7e0) = param_1;
}

// RH_ScopedInstall(TimerStateSet, 0x0041e130);  // first-wins; superseded
// (// frida-sweep first-wins (TimerInit.cpp/AudioRws.cpp registered first): 0x0041e130)

// ── 0x00426630 ───────────────────────────────────────────────────────────────
// PitchParamSet — writes param_1 to DAT_0066d6fc.
// Body (9 bytes):
//   A3 FC D6 66 00  MOV [0x0066d6fc], EAX
//   C3              RET
static constexpr std::uintptr_t kPitchParam0066d6fc = 0x0066d6fc;

extern "C" __declspec(dllexport) void __cdecl PitchParamSet(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(kPitchParam0066d6fc) = param_1;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(PitchParamSet, 0x00426630);

// ── 0x004266f0 ───────────────────────────────────────────────────────────────
// PitchParam2Set — writes param_1 to DAT_0066d700 (sibling of PitchParamSet; target +4).
// Body (9 bytes):
//   A3 00 D7 66 00  MOV [0x0066d700], EAX
//   C3              RET
static constexpr std::uintptr_t kPitchParam20066d700 = 0x0066d700;

extern "C" __declspec(dllexport) void __cdecl PitchParam2Set(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(kPitchParam20066d700) = param_1;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(PitchParam2Set, 0x004266f0);
