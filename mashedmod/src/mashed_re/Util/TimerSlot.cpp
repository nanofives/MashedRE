// Mashed RE — Timer slot helpers: ZeroFillWrapper.
// Session: c3-batch-e-s13 (2026-05-15)
//
// Drift-promote (C1->C2):
//   0x004b6520  FUN_004b6520  ZeroFillWrapper
//     20-byte wrapper: FUN_004b64e0(param_1, 0, param_2)
//     FUN_004b64e0 is a word-at-a-time memset(dest, fill, count).
//     Entire wrapper equivalent to: memset(param_1, 0, param_2).
//
// dualcopy4 (2026-10-16, ROADMAP D4): this file used to carry a SECOND body for three RVAs that
// Util/TimerInit.cpp already owns and installs. Those copies (SlotDataCopy 0x0041f000, SlotBitSet
// 0x0041eda0, SlotArrayClear 0x00420d40) were never installed ("first-wins; superseded"), had no caller in
// either target, and their registry keys slot_bit_set / slot_array_clear are retired. The one body of each
// is now in Util/TimerInit.cpp: TimerSlotDataCopy, TimerBitFieldSet, TimerStructArrayClear.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────
// Drift-promote: 0x004b6520  ZeroFillWrapper
// ─────────────────────────────────────────────────────────────────────────────
// Wraps FUN_004b64e0(param_1, 0, param_2) — zero-fills param_2 bytes at param_1.
// Analysis note: re/analysis/input_lua_d2/0x004b6520.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ZeroFillWrapper(void* param_1, unsigned int param_2) {
    std::memset(param_1, 0, param_2);
}

RH_ScopedInstall(ZeroFillWrapper, 0x004b6520);  // re-enabled 2026-05-24 phase-a2-strict GREEN bytes_inplace
