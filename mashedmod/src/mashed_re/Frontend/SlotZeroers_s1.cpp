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

// 0x00422a80
extern "C" __declspec(dllexport) void __cdecl SlotBlockZero(int param_1)
{
    // Compute destination: &DAT_006403e8 + param_1 * 0xf40. [0x00422a8d]
    void* dst = reinterpret_cast<void*>(
        0x006403e8u + static_cast<std::uint32_t>(param_1) * 0xf40u);

    // Zero 0xf40 bytes via ZeroFillWrapper. [0x00422a8d]
    s_ZeroFillWrapper(dst, 0xf40u);
}

RH_ScopedInstall(SlotBlockZero, 0x00422a80);

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

// 0x00422aa0
extern "C" __declspec(dllexport) void __cdecl SlotFieldSet(
    int param_1, std::uint32_t param_2)
{
    // Write param_2 to field at DAT_0064131c + param_1 * 0xf40. [0x00422aa5]
    // DAT_0064131c = DAT_006403e8 + 0xf34 (field at offset +0xf34 within 0xf40-byte slot).
    *reinterpret_cast<std::uint32_t*>(
        0x0064131cu + static_cast<std::uint32_t>(param_1) * 0xf40u) = param_2;
}

RH_ScopedInstall(SlotFieldSet, 0x00422aa0);

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

    // Per-axis repeat-key logic. Each axis: timer at 0x006440ec[axis_offset].
    // LEFT (DAT_007f1044) [0x004230b0]
    {
        std::int32_t* pTimer  = reinterpret_cast<std::int32_t*>(0x006440fcu);
        std::int32_t* pTimer2 = reinterpret_cast<std::int32_t*>(0x00644108u);
        float*        pScroll = reinterpret_cast<float*>(0x007f1a5cu);
        std::uint8_t  flag    = *reinterpret_cast<std::uint8_t*>(0x007f1044u);
        if (!flag) {
            *pTimer  = 0;
            *pTimer2 = 0;
        } else {
            if (*pTimer2 == 0) {
                *pScroll -= step;
            }
            if (*pTimer == 0) {
                *pTimer  = 10;  // initial repeat delay [0x00423113]
                *pTimer2 = 1;
            } else {
                (*pTimer)--;
                if (*pTimer == 0) {
                    *pTimer2 = 2;
                }
            }
        }
    }
    // RIGHT (DAT_007f1045) [0x00423152]
    {
        std::int32_t* pTimer  = reinterpret_cast<std::int32_t*>(0x006440ecu);
        std::int32_t* pTimer2 = reinterpret_cast<std::int32_t*>(0x006440f0u);
        float*        pScroll = reinterpret_cast<float*>(0x007f1a5cu);
        std::uint8_t  flag    = *reinterpret_cast<std::uint8_t*>(0x007f1045u);
        if (!flag) {
            *pTimer  = 0;
            *pTimer2 = 0;
        } else {
            if (*pTimer2 == 0) {
                *pScroll += step;
            }
            if (*pTimer == 0) {
                *pTimer  = 10;
                *pTimer2 = 1;
            } else {
                (*pTimer)--;
                if (*pTimer == 0) {
                    *pTimer2 = 2;
                }
            }
        }
    }
    // UP (DAT_007f1046) [0x004231b0]
    {
        std::int32_t* pTimer  = reinterpret_cast<std::int32_t*>(0x006440f4u);
        std::int32_t* pTimer2 = reinterpret_cast<std::int32_t*>(0x006440f8u);
        float*        pScroll = reinterpret_cast<float*>(0x007f1a58u);
        std::uint8_t  flag    = *reinterpret_cast<std::uint8_t*>(0x007f1046u);
        if (!flag) {
            *pTimer  = 0;
            *pTimer2 = 0;
        } else {
            if (*pTimer2 == 0) {
                *pScroll += step;
            }
            if (*pTimer == 0) {
                *pTimer  = 10;
                *pTimer2 = 1;
            } else {
                (*pTimer)--;
                if (*pTimer == 0) {
                    *pTimer2 = 2;
                }
            }
        }
    }
    // DOWN (DAT_007f1047) [0x004231fb]
    {
        std::int32_t* pTimer  = reinterpret_cast<std::int32_t*>(0x006440fcu);
        std::int32_t* pTimer2 = reinterpret_cast<std::int32_t*>(0x00644104u);
        float*        pScroll = reinterpret_cast<float*>(0x007f1a58u);
        std::uint8_t  flag    = *reinterpret_cast<std::uint8_t*>(0x007f1047u);
        if (!flag) {
            *pTimer  = 0;
            *pTimer2 = 0;
        } else {
            if (*pTimer2 == 0) {
                *pScroll -= step;
            }
            if (*pTimer == 0) {
                *pTimer  = 10;
                *pTimer2 = 1;
            } else {
                (*pTimer)--;
                if (*pTimer == 0) {
                    *pTimer2 = 2;
                }
            }
        }
    }
}

// RH_ScopedInstall(FrontendDirInput, 0x00423040);
// CALLEE-GATE CLEARED (00417450/00417530 C2 frontend-gate-unblock-u 2026-05-26)
// but DIFF RED 8/10 (frontend-gate-unblock-u): this reimpl's per-axis repeat-timer
// slot addresses (DAT_006440ec.. mapping) DIVERGE from the original — orig writes
// the timer/state values to different slots than this impl assumes. The
// state_machine_observe harness (flags 0x007f1042/0x007f1076 held 0) is correct;
// the BUG is in the timer-address mapping above. Install kept OFF until the exact
// store offsets are re-read from Ghidra (listing of 0x004230b0..0x00423240 stores)
// and the LEFT/RIGHT/UP/DOWN pTimer/pTimer2 addresses corrected. Evidence:
// log/diff_frontend_dir_input.csv. Re-pickup: Ghidra re-analysis of store offsets.

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

// 0x00423270
extern "C" __declspec(dllexport) void __cdecl TabCycler()
{
    std::int32_t context = *reinterpret_cast<std::int32_t*>(0x007f1a54u);

    // Select per-context bounds. [0x0042327d..0x004232bc]
    int lo = 0;
    int hi = 8;  // default [0x0042327d]
    if (context >= 1 && context <= 6) {
        lo = 2;  // low bound for all non-default contexts [0x004232bc]
        switch (context) {
            case 1: hi = 3; break;  // [0x0042328d]
            case 2: hi = 4; break;  // [0x0042329b]
            case 3: hi = 3; break;  // [0x004232a9]
            case 4: hi = 5; break;  // [0x004232b4]
            case 5: hi = 3; break;  // [0x004232b8]
            case 6: hi = 5; break;  // [0x004232b4]
        }
    }

    std::int32_t* pTab    = reinterpret_cast<std::int32_t*>(0x007f1a64u);
    std::uint8_t  tabR    = *reinterpret_cast<std::uint8_t*>(0x007f1071u);
    std::uint8_t  tabL    = *reinterpret_cast<std::uint8_t*>(0x007f1070u);
    std::uint8_t  debR    = *reinterpret_cast<std::uint8_t*>(0x007f1531u);
    std::uint8_t  debL    = *reinterpret_cast<std::uint8_t*>(0x007f1530u);

    // Tab-right: increment with wrap. [0x004232be]
    if (tabR && !debR) {
        std::int32_t v = *pTab + 1;
        if (v > hi) v = lo;
        *pTab = v;
    }

    // Tab-left: decrement with wrap. [0x004232d8]
    if (tabL && !debL) {
        std::int32_t v = *pTab - 1;
        if (v < lo) v = hi;
        *pTab = v;
    }
}

RH_ScopedInstall(TabCycler, 0x00423270);
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

// 0x00423320
extern "C" __declspec(dllexport) void __cdecl CursorMover()
{
    std::uint32_t* pCursor = reinterpret_cast<std::uint32_t*>(0x007f1a90u);
    std::int32_t   tab     = *reinterpret_cast<std::int32_t*>(0x007f1a64u);

    std::uint32_t uVar1 = *pCursor;

    // Up/down guarded by tab != 2. [0x00423331]
    if (tab != 2) {
        // UP (DAT_007f1072 && !DAT_007f1532): [0x00423345]
        if (*reinterpret_cast<std::uint8_t*>(0x007f1072u) &&
            !*reinterpret_cast<std::uint8_t*>(0x007f1532u)) {
            // uVar1 = cursor - 4; clamp at 0. [0x00423345]
            std::int32_t v = static_cast<std::int32_t>(uVar1) - 4;
            if (v < 0) v = static_cast<std::int32_t>(uVar1);
            uVar1 = static_cast<std::uint32_t>(v);
        }
        *pCursor = uVar1;

        // DOWN (DAT_007f1073 && !DAT_007f1533): [0x00423365]
        if (*reinterpret_cast<std::uint8_t*>(0x007f1073u) &&
            !*reinterpret_cast<std::uint8_t*>(0x007f1533u)) {
            // uVar1 = cursor + 4; clamp at 0x7f. [0x00423365]
            std::int32_t v = static_cast<std::int32_t>(uVar1) + 4;
            if (v > 0x7f) v = static_cast<std::int32_t>(uVar1);
            uVar1 = static_cast<std::uint32_t>(v);
        }
    }
    *pCursor = uVar1;

    // LEFT (DAT_007f1074 && !DAT_007f1534): horizontal wrap within row. [0x00423389]
    if (*reinterpret_cast<std::uint8_t*>(0x007f1074u) &&
        !*reinterpret_cast<std::uint8_t*>(0x007f1534u)) {
        std::uint32_t v = uVar1 - 1u;
        if ((v & 3u) == 3u) {
            // Wrapped past column 0: jump to column 3 of same row.
            *pCursor = uVar1 + 3u;
        } else {
            *pCursor = v;
        }
        uVar1 = *pCursor;
    }

    // RIGHT (DAT_007f1075 && !DAT_007f1535): horizontal wrap within row. [0x00423389+]
    if (*reinterpret_cast<std::uint8_t*>(0x007f1075u) &&
        !*reinterpret_cast<std::uint8_t*>(0x007f1535u)) {
        std::uint32_t v = uVar1 + 1u;
        if ((v & 3u) == 0u) {
            // Wrapped past column 3: jump back to column 0 of same row.
            *pCursor = uVar1 - 3u;
        } else {
            *pCursor = v;
        }
    }
}

RH_ScopedInstall(CursorMover, 0x00423320);
// Harness extension landed 2026-05-26: 'state_machine_observe' arg_type covers this.
