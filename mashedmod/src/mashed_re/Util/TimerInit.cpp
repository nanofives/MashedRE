// Mashed RE — timer slot init setters.
// 0x0041d820  FUN_0041d820  timer_d3_cont1_b  C2 (promote_c2_util_timer-20260513)
// 0x0041e130  FUN_0041e130  timer_d3_cont1_b  C2 (promote_c2_util_timer-20260513)
#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0041d820 — TimerSlotClear
//
// Writes constant 0 to DAT_0063d558.
// 10-byte body: no branches, no calls. Single MOV + RET.
//
// Asm:
//   0041d820: C7 05 58 D5 63 00 00 00 00 00  MOV DWORD PTR [0x0063d558], 0x0
//   0041d82a: C3                             RET
//
// Only caller: FUN_004111c0 (large init function at 0x004111c0).
// DAT_0063d558 is also the upper-bound sentinel of the loop in
// FUN_0041d730 (0x0041d730). [UNCERTAIN U-1XXX — role unclear;
// does not affect correctness of this single-write reimplementation.]
extern "C" __declspec(dllexport) void __cdecl TimerSlotClear() {
    *reinterpret_cast<std::uint32_t*>(0x0063d558) = 0u;
}

// 0x0041e130 — TimerTrackSetter
//
// Writes param_1 (undefined4) to DAT_0063d7e0.
// 9-byte body: no branches, no calls. Single MOV + RET.
//
// Asm:
//   0041e130: 8B 44 24 04  MOV EAX, DWORD PTR [ESP+0x4]   ; param_1
//   0041e134: A3 E0 D7 63 00  MOV [0x0063d7e0], EAX
//   0041e139: C3              RET
//
// Callers: FUN_004111c0 (0x004111c0) and FUN_0040e560 (0x0040e560).
// [UNCERTAIN U-1XXX — type and semantics of DAT_0063d7e0 unconfirmed;
// does not affect offset/write correctness.]
extern "C" __declspec(dllexport) void __cdecl TimerTrackSetter(std::uint32_t param_1) {
    *reinterpret_cast<std::uint32_t*>(0x0063d7e0) = param_1;
}

RH_ScopedInstall(TimerSlotClear,   0x0041d820);
RH_ScopedInstall(TimerTrackSetter, 0x0041e130);
