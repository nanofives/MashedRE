// Mashed RE - c3_batch_ah s5: audio root/timer getters.
// Bit-faithful ports of audio-module pure leaves. Decomp pool14 2026-06-08
// (matches re/analysis/bucket_audio_005a7b60_005ab620/{005aad40,005ab070}.md).
//
// DEFERRED in this session (not authored here):
//   0x005ab010 AudioElapsedTicks      - reads a MOVING counter (FUN_005ab040)
//       minus a baseline global; read_global seeds one global but cannot
//       capture a moving-counter diff deterministically -> racy/non-det.
//   0x005aaff0 AudioTimerBaselineInit - writes counter@now into dcdf0 and 0
//       into dcdf4; void_write_observe on dcdf4 is degenerate (always 0),
//       on dcdf0 is racy (counter moves between orig/reimpl calls).
//   (2026-06-08 harness/scenario-attach) the three voice field-getters below
//   are now promotable via the new thiscall_field_get arg_type (synthetic
//   struct-seed, plain menu-attach) — authored + registered here.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005aad40  FUN_005aad40  (6 bytes)  undefined4(void)
// MOV EAX,[0x007dcd20]; RET. Audio root/system object pointer getter.
extern "C" __declspec(dllexport) uint32_t __cdecl AudioRootGet(void) {
    return *reinterpret_cast<volatile uint32_t*>(0x007dcd20);
}

RH_ScopedInstall(AudioRootGet, 0x005aad40);  // c3_batch_ah s5

// 0x005ab070  FUN_005ab070  (21 bytes)  undefined4(void)
// uVar1 = DAT_007dcdf8; if (DAT_007dce00 != 1) uVar1 = 1000; return uVar1.
// Timer tick-rate getter; literal 1000 fallback when mode flag != 1.
extern "C" __declspec(dllexport) uint32_t __cdecl AudioTimerRateGet(void) {
    uint32_t uVar1 = *reinterpret_cast<volatile uint32_t*>(0x007dcdf8);
    if (*reinterpret_cast<volatile uint32_t*>(0x007dce00) != 1) {
        uVar1 = 1000;
    }
    return uVar1;
}

RH_ScopedInstall(AudioTimerRateGet, 0x005ab070);  // c3_batch_ah s5

// The voice field getters 0x005a89a0 / 0x005a89b0 / 0x005a89c0 and the channel getter 0x005ab370 moved to
// Audio/AudioFieldGetters.cpp (audiolink slice 5, one body in both targets). This file stays asi-only: AudioRootGet /
// AudioTimerRateGet read globals the exe has no writer for.
// AudioLinearToLog 0x005aeb90 moved to Audio/AudioLinearToLog.cpp (D4-AUDIO-VOLTODS, both targets).
