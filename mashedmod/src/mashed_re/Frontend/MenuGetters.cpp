// Mashed RE - Frontend menu getter reimplementations.
// Analysis note: re/analysis/frontend_promote_menus_a/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// MenuAlphaGet  --  0x0042b930
//
// Original: FUN_0042b930 (5 bytes, 0x0042b930..0x0042b935)
// Returns: DAT_0067ecb0 (uint32_t global, read and returned directly).
// No callees, no branches, no side-effects.
// ref: re/analysis/frontend_promote_menus_a/0x0042b930.md
// ---------------------------------------------------------------------------

// 0x0042b930
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuAlphaGet() {
    return *reinterpret_cast<std::uint32_t*>(0x0067ecb0u);
}

RH_ScopedInstall(MenuAlphaGet, 0x0042b930);  // re-enabled 2026-05-24 batch-frontend

// ---------------------------------------------------------------------------
// MenuGroupCount  --  0x0042ac00
//
// Original: FUN_0042ac00 (67 bytes, 0x0042ac00..0x0042ac43)
// Signature: int __fastcall FUN_0042ac00(undefined4 param_1, int* param_2)
//   ECX = param_1 (unused in body; __fastcall ABI artifact)
//   EDX = param_2 (int*, pointer to sentinel-delimited int array)
// Returns: count of 0xFF060000 delimiter records before the 0xFF070000 terminator.
// Side-effects: none (read-only scan).
//
// Sentinel scheme (cited from within 0x0042ac00):
//   0xFF060000 = group delimiter
//   0xFF070000 = end-of-array terminator
//
// ref: re/analysis/frontend_promote_menus_a/0x0042ac00.md
// ---------------------------------------------------------------------------

// 0x0042ac00
// BUGFIX 2026-07-26 — ABI MISMATCH; third confirmed pc=0x44 boot-AV crasher
// (MASHED_HOOK_ONLY=MenuGroupCount alone: 4/4 AV 0xC0000005 @ 8.6-10.9 s).
//
// The original is __fastcall (ECX = param_1 unused, EDX = param_2). This reimpl was
// declared __cdecl, which reads BOTH args off the stack. RH_ScopedInstall redirects
// the original address, so the GAME's callers still pass param_2 in EDX and push
// nothing — our __cdecl body therefore read stack garbage as `param_2` and walked a
// wild pointer through the sentinel scan.
//
// The old comment justified __cdecl on the grounds that Frida can call the original
// as __fastcall — true, but irrelevant: it describes the DIFF HARNESS's call, not the
// installed inline-JMP's real callers. This is the "synthetic diff passes while the
// installed hook diverges" trap (memory feedback_diff_reimpl_asm_vs_original); the row
// was even marked "sentinel_array_ptr GREEN (11/11)".
//
// NOTE: hooks_registry.py already recorded this fix on 2026-06-01
// ("reimpl rebuilt as __fastcall ... was 'mscdecl', which let orig+reimpl each pass
// under its own convention while the LIVE hook crashed (boot AV)") — but the source
// change never landed. The registry claimed fastcall while the code stayed cdecl.
// Registry `export` updated to the decorated '@MenuGroupCount@8' to match.
extern "C" __declspec(dllexport) int __fastcall MenuGroupCount(int /*ecx_unused*/, int* param_2) {
    int count = 0;
    if (!param_2) return 0;
    while (true) {
        int v = *param_2;
        while (v != 0xFF060000) {
            if (v == static_cast<int>(0xFF070000u)) return count;
            ++param_2;
            v = *param_2;
        }
        // at a 0xFF060000 delimiter
        if (*param_2 == static_cast<int>(0xFF070000u)) return count;
        ++count;
        ++param_2;
    }
}

// RH_ScopedInstall patches the original __fastcall at 0x0042ac00; our reimpl is now
// __fastcall too, so the installed inline-JMP and the game's callers agree on the ABI.
RH_ScopedInstall(MenuGroupCount, 0x0042ac00);  // ABI fixed 2026-07-26 (was __cdecl -> live boot AV)

// ---------------------------------------------------------------------------
// MenuCursorStep  --  0x0042aa00
//
// Original: FUN_0042aa00 (168 bytes, 0x0042aa00..0x0042aaa8)
// Signature: void FUN_0042aa00(int param_1)
//   param_1 = step (positive=forward, negative=backward)
// Side-effects: reads/writes cursor at 0x0067ed40+slot*0x40; writes 0xffffffff
//   on exhaustion.
//
// Memory layout (cited from within 0x0042aa00):
//   slot index:   DAT_0067e9f8      (int32, stride global)
//   limit array:  0x0067ed74 + slot*0x40   (int32 per slot)
//   cursor array: 0x0067ed40 + slot*0x40   (int32 per slot)
//   validity:     0x0067ed84 + cursor + slot*0x10 - 0x10  (int8, 1=valid)
//
// ref: re/analysis/frontend_promote_menus_a/0x0042aa00.md
// ---------------------------------------------------------------------------

// 0x0042aa00
extern "C" __declspec(dllexport) void __cdecl MenuCursorStep(int param_1) {
    int slot = *reinterpret_cast<std::int32_t*>(0x0067e9f8u);
    int slotOff40 = slot * 0x40;
    int slotOff10 = slot * 0x10;

    int limit = *reinterpret_cast<std::int32_t*>(0x0067ed74u + slotOff40);
    if (limit == 0) {
        *reinterpret_cast<std::uint32_t*>(0x0067ed40u + slotOff40) = 0xFFFFFFFFu;
        return;
    }

    bool found = false;
    int iIter = 0;
    while (true) {
        int cursor = *reinterpret_cast<std::int32_t*>(0x0067ed40u + slotOff40);
        cursor += param_1;
        if (cursor == limit) {
            cursor = 0;
        } else if (cursor < 0) {
            cursor = limit - 1;
        }
        *reinterpret_cast<std::int32_t*>(0x0067ed40u + slotOff40) = cursor;

        if (*reinterpret_cast<std::int8_t*>(0x0067ed84u + cursor + slotOff10 - 0x10) == 1) {
            found = true;
        }
        ++iIter;
        if (limit < iIter) break;
        if (found) return;
    }
    *reinterpret_cast<std::uint32_t*>(0x0067ed40u + slotOff40) = 0xFFFFFFFFu;
}

// RH_ScopedInstall(MenuCursorStep, 0x0042aa00);
// Disabled: integration diff RED (2026-05-15). Validity-address formula
// 0x0067ed84+cursor+slot*0x10-0x10 doesn't match original; Ghidra re-check needed.
