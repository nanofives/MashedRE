// Mashed RE — Frontend slot-zero utilities and slot-state setters (c3-batch-s session 1).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00422a80  SlotBlockZero     — 8B leaf; zeroes 0xf40 bytes at DAT_006403e8[slot]
//   0x00422aa0  SlotFieldSet      — 15B leaf; writes dword at DAT_0064131c[slot*0xf40]
//   0x00423040  FrontendDirInput  — directional-input handler; callees C1 (NOT promoted)
//   0x00423270  TabCycler         — menu tab cycler; void(void); harness-ext needed
//   0x00423320  CursorMover       — grid cursor movement; void(void); harness-ext needed
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s2/0x00422a80.md
//   re/analysis/frontend_c1_to_c2_s2/0x00422aa0.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423040.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423270.md
//   re/analysis/frontend_c1_to_c2_s2/0x00423320.md

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Forward declarations for callees used in this file
// ---------------------------------------------------------------------------

// 0x004b6520  ZeroFillWrapper  — C2 (Util/TimerSlot.cpp)
// void __cdecl ZeroFillWrapper(void* dst, std::uint32_t size)
// Zeroes `size` bytes starting at `dst`.
static auto* const s_ZeroFillWrapper =
    reinterpret_cast<void(__cdecl*)(void*, std::uint32_t)>(0x004b6520);

// ---------------------------------------------------------------------------
// SlotBlockZero  --  0x00422a80
//
// Original: FUN_00422a80 (8 bytes, 0x00422a80..0x00422a97)
// Signature: void FUN_00422a80(int param_1)
//   param_1: slot index (int)
// Returns: void
//
// Body (mechanical transcript):
//   param_1 * 0xf40  → byte offset into array at 0x006403e8
//   FUN_004b6520(&DAT_006403e8 + param_1 * 0xf40, 0xf40)   // @ 0x00422a8d
//   return
//
// Constants (cited from 0x00422a80 body):
//   0x006403e8 — array base                                  [0x00422a8d]
//   0xf40      — element stride / zero size (3904 decimal)   [0x00422a8d]
//
// Callee: FUN_004b6520 (ZeroFillWrapper) is C2.
//
// NOTE: Harness extension needed for Frida promotion.
//   No existing arg_type supports save+call+observe for a single-int slot-zero.
//   A new arg_type 'slot_block_zero' would: pre-fill sentinel at base+slot*stride,
//   call fn(slot), read back first dword (should be 0). Queued in PROMOTION_QUEUE.md.
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00422a80.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00422a80 (SlotBlockZero) moved to Match/MatchS05_00422a80.cpp,
// which compiles the byte-matched source re/match/s05/00422a80.c.

// ---------------------------------------------------------------------------
// SlotFieldSet  --  0x00422aa0
//
// Original: FUN_00422aa0 (15 bytes, 0x00422aa0..0x00422aae)
// Signature: void FUN_00422aa0(int param_1, undefined4 param_2)
//   param_1: slot index (int)
//   param_2: value to write (uint32_t / undefined4 / 4-byte)
// Returns: void
//
// Body (pure leaf, no calls):
//   *(undefined4*)(&DAT_0064131c + param_1 * 0xf40) = param_2   // @ 0x00422aa5
//   return
//
// Constants (cited from 0x00422aa0 body):
//   0x0064131c  — field base (= DAT_006403e8 + 0xf34)   [0x00422aa5]
//   0xf40       — stride (3904 decimal)                  [0x00422aa5]
//
// Pure leaf — no callees.
// arg_type: 'entity_field_set' — fn(p1, p2), reads back
//   CONFIG.target_global + p1 * CONFIG.entity_byte_stride as observable uint32.
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00422aa0.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00422aa0 (SlotFieldSet) moved to Match/MatchS05_00422aa0.cpp,
// which compiles the byte-matched source re/match/s05/00422aa0.c.

// ---------------------------------------------------------------------------
// FrontendDirInput  --  0x00423040
//
// Original: FUN_00423040 (~280 bytes, 0x00423040..0x00423258)
// Signature: void FUN_00423040(void)
// Returns: void
//
// Body summary: frontend directional-input handler with per-axis repeat-key
// logic (4 axes: left/right/up/down). Conditionally fires two callbacks
// (FUN_00417450, FUN_00417530) when specific button combos match menu state
// and tab index.
//
// CALLEE STATUS BLOCK — NOT PROMOTED TO C3:
//   FUN_00417450 @ 0x00417450 — C1 (gameplay subsystem)
//   FUN_00417530 @ 0x00417530 — C1 (gameplay subsystem)
// Anti-island rule: cannot promote until both callees are C2+.
//
// Implementation included here for completeness; RH_ScopedInstall is
// COMMENTED OUT until callee condition is met.
//
// Globals referenced:
//   DAT_007f1042 — button flag (byte)
//   DAT_007f1044..DAT_007f1047 — L/R/U/D input byte flags [0x004230b0..0x004231fb]
//   DAT_007f1076 — second button flag (byte)
//   DAT_007f1a54 — menu state int
//   DAT_007f1a64 — tab index int
//   DAT_007f1a58, DAT_007f1a5c — scroll/cursor position floats
//   DAT_006440ec..DAT_0064410c — per-direction repeat-timer counters (int)
//   DAT_005cc564 — step scalar (float) [0x0042310e]
//
// Constants (cited):
//   -0x14 (-20) — x-offset base [0x00423076]
//   -0xc  (-12) — y-offset base [0x00423076]
//   10 (0xa)    — initial repeat delay [0x00423113]
//   1           — repeat interval [0x0042312b]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423040.md
// ---------------------------------------------------------------------------

static auto* const s_FUN_00417450 =
    reinterpret_cast<void(__cdecl*)(int, int, int)>(0x00417450);
static auto* const s_FUN_00417530 =
    reinterpret_cast<void(__cdecl*)(int, int)>(0x00417530);

// 0x00423040
extern "C" __declspec(dllexport) void __cdecl FrontendDirInput()
{
    // External callees for FPU pixel-offset computation — C2 (FrontendAccessors.cpp area).
    auto* const s_FUN_004a2c48 =
        reinterpret_cast<int(__cdecl*)()>(0x004a2c48);

    // Step 1: compute pixel offsets. [0x00423048, 0x0042304e]
    int iVar1 = s_FUN_004a2c48();
    int iVar2 = s_FUN_004a2c48();

    std::uint8_t* const pState = reinterpret_cast<std::uint8_t*>(0x007f1042u);
    std::int32_t menu_state    = *reinterpret_cast<std::int32_t*>(0x007f1a54u);
    std::int32_t tab_index     = *reinterpret_cast<std::int32_t*>(0x007f1a64u);

    // Button 0x1042 + menu_state==0 + tab==4 → call callback A. [0x00423076]
    if (pState[0] && menu_state == 0 && tab_index == 4) {
        std::uint8_t bits = *reinterpret_cast<std::uint8_t*>(0x007f1a90u) & 3u;
        s_FUN_00417450(-0x14 - iVar1, -0xc - iVar2, static_cast<int>(bits));
    }
    // Button 0x1076 + menu_state==0 + tab==4 → call callback B. [0x004230a0]
    if (*reinterpret_cast<std::uint8_t*>(0x007f1076u) && menu_state == 0 && tab_index == 4) {
        s_FUN_00417530(-0x14 - iVar1, -0xc - iVar2);
    }

    float step = *reinterpret_cast<float*>(0x005cc564u);

    // Per-axis repeat-key logic (corrected against Ghidra decomp 0x004230b0..0x0042326b,
    // frontend-gate-unblock-u 2026-05-26). Each axis has a PHASE word (0/1/2 state) and a
    // COUNT word (countdown 10→0). Logic per axis (P=phase, C=count):
    //   flag==0          → P=0; C=0
    //   flag!=0:  if C==0 → scroll ±= step
    //             if P==0 → P=1; C=10
    //             else    → P=2; if C==0 → C=1; else C--; if C<0 → C=0  (P stays 2)
    auto repeat_axis = [step](std::uint8_t flag, std::int32_t* P, std::int32_t* C,
                              float* scroll, bool plus) {
        if (!flag) { *P = 0; *C = 0; return; }
        if (*C == 0) { *scroll += plus ? step : -step; }
        if (*P == 0) { *P = 1; *C = 10; }
        else {
            *P = 2;
            if (*C == 0) { *C = 1; }
            else { (*C)--; if (*C < 0) *C = 0; }
        }
    };
    // LEFT  (007f1044): phase 006440fc, count 00644108, scroll 007f1a5c, minus [0x004230b0]
    repeat_axis(*reinterpret_cast<std::uint8_t*>(0x007f1044u),
                reinterpret_cast<std::int32_t*>(0x006440fcu),
                reinterpret_cast<std::int32_t*>(0x00644108u),
                reinterpret_cast<float*>(0x007f1a5cu), false);
    // RIGHT (007f1045): phase 0064410c, count 006440f4, scroll 007f1a5c, plus  [0x00423152]
    repeat_axis(*reinterpret_cast<std::uint8_t*>(0x007f1045u),
                reinterpret_cast<std::int32_t*>(0x0064410cu),
                reinterpret_cast<std::int32_t*>(0x006440f4u),
                reinterpret_cast<float*>(0x007f1a5cu), true);
    // UP    (007f1046): phase 006440ec, count 00644104, scroll 007f1a58, plus  [0x004231b0]
    repeat_axis(*reinterpret_cast<std::uint8_t*>(0x007f1046u),
                reinterpret_cast<std::int32_t*>(0x006440ecu),
                reinterpret_cast<std::int32_t*>(0x00644104u),
                reinterpret_cast<float*>(0x007f1a58u), true);
    // DOWN  (007f1047): phase 006440f0, count 006440f8, scroll 007f1a58, minus [0x004231fb]
    repeat_axis(*reinterpret_cast<std::uint8_t*>(0x007f1047u),
                reinterpret_cast<std::int32_t*>(0x006440f0u),
                reinterpret_cast<std::int32_t*>(0x006440f8u),
                reinterpret_cast<float*>(0x007f1a58u), false);
}

RH_ScopedInstall(FrontendDirInput, 0x00423040);
// Per-axis timer logic CORRECTED against Ghidra decomp (frontend-gate-unblock-u
// 2026-05-26): the earlier impl had swapped phase/count roles and wrong addresses
// (RED 8/10). Now uses the verified per-axis phase/count mapping. Callees
// 00417450/00417530 C2; diffed via state_machine_observe (callback flags held 0).

// ---------------------------------------------------------------------------
// TabCycler  --  0x00423270
//
// Original: FUN_00423270 (~120 bytes, 0x00423270..0x004232ef)
// Signature: void FUN_00423270(void)
// Returns: void
//
// Body: selects per-context [low, high] tab bounds from DAT_007f1a54,
// then increments/decrements DAT_007f1a64 with cyclic wrap on rising edge
// of tab-right (DAT_007f1071) / tab-left (DAT_007f1070) buttons.
//
// Pure leaf — no callees.
//
// NOTE: Harness extension needed for Frida promotion.
//   Needs a new arg_type 'tab_cycler_observe' that:
//     1. Injects: DAT_007f1a54 (context), DAT_007f1071, DAT_007f1070 (button flags),
//        DAT_007f1531, DAT_007f1530 (debounce), DAT_007f1a64 (initial tab index).
//     2. Calls fn() (void, no args).
//     3. Reads back DAT_007f1a64 as the observable output.
//     4. Restores all injected globals.
//   Queued in PROMOTION_QUEUE.md.
//
// Constants:
//   default high bound: 8 [0x0042327d]
//   context 1 → high=3 [0x0042328d], context 2 → high=4 [0x0042329b]
//   context 3 → high=3 [0x004232a9], context 4/6 → high=5 [0x004232b4]
//   context 5 → high=3 [0x004232b8], low bound (contexts 1-6) = 2 [0x004232bc]
//   tab-right flag: DAT_007f1071 [0x004232be], tab-left: DAT_007f1070 [0x004232d8]
//   tab index: DAT_007f1a64 [0x004232c5]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423270.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423270 (TabCycler) moved to Match/MatchS05_00423270.cpp,
// which compiles the byte-matched source re/match/s05/00423270.c.
// Harness extension landed 2026-05-26: 'state_machine_observe' arg_type covers this.

// ---------------------------------------------------------------------------
// CursorMover  --  0x00423320
//
// Original: FUN_00423320 (~130 bytes, 0x00423320..0x004233a2)
// Signature: void FUN_00423320(void)
// Returns: void
//
// Body: 8-way cursor movement on a 4-column × 32-row grid.
//   Up/down: stride 4, clamped to [0, 127]. Guarded by tab != 2.
//   Left/right: horizontal wrap within 4-column row.
//   Debounce: each direction has its own debounce byte.
//
// Pure leaf — no callees.
//
// NOTE: Harness extension needed for Frida promotion.
//   Needs a new arg_type 'cursor_grid_observe' that:
//     1. Injects: DAT_007f1a64 (tab), DAT_007f1a90 (cursor), DAT_007f1072..1075
//        (direction flags), DAT_007f1532..1535 (debounce flags).
//     2. Calls fn() (void).
//     3. Reads back DAT_007f1a90 as the observable output.
//     4. Restores all injected globals.
//   Queued in PROMOTION_QUEUE.md.
//
// Constants:
//   row stride: 4 [0x00423345], max cursor index: 0x7f (127) [0x00423365]
//   column mask: 3 [0x00423389], tab guard value: 2 [0x00423331]
//   cursor global: DAT_007f1a90 [0x00423328]
//
// ref: re/analysis/frontend_c1_to_c2_s2/0x00423320.md
// ---------------------------------------------------------------------------

// MATCH-S05: the body of 0x00423320 (CursorMover) moved to Match/MatchS05_00423320.cpp,
// which compiles the byte-matched source re/match/s05/00423320.c.
// Harness extension landed 2026-05-26: 'state_machine_observe' arg_type covers this.
