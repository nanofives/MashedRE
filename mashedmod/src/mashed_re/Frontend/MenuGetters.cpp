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
// Exported __cdecl; original is __fastcall(ECX=unused, EDX=param_2).
// ECX (param_1) is never read in the original body, so calling the original
// with __fastcall from Frida (ECX=0, EDX=ptr) is equivalent.
// Our reimpl is __cdecl with an explicit dummy first arg so the function body
// is structurally identical and the export name is undecorated.
extern "C" __declspec(dllexport) int __cdecl MenuGroupCount(int /*param_unused*/, int* param_2) {
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

// Note: RH_ScopedInstall patches the original __fastcall at 0x0042ac00.
// Our reimpl is __cdecl but functionally identical (param_1/ECX unused).
// MASS-DISABLED 2026-05-24 c3-refused-needs-arg-type: RH_ScopedInstall(MenuGroupCount, 0x0042ac00);

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
