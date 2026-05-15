// Mashed RE — Sprite table lookup forwarders and HUD slot-type mappers.
// Analysis notes: re/analysis/sprite_gate_c3/
//
// Covers:
//   0x0040bb70  SpriteLookupTableA  — forwarder: FUN_004c5c00(DAT_0063b900, param_1)
//   0x0040bb90  SpriteLookupTableB  — forwarder: FUN_004c5c00(DAT_0063b904, param_1)
//   0x0042ee00  SpriteSlotGate      — calls SpriteLookupTableA_orig for slot 0/1/2, else 0
//   0x00430a10  HudSlotTypePlayer0  — game-mode → slot-type-0 (leaf, reads DAT_0067e9fc)
//   0x00430a60  HudSlotTypePlayer1  — game-mode → slot-type-1 (leaf)
//   0x00430ab0  HudSlotTypePlayer2  — game-mode → slot-type-2 (leaf)
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: FUN_004c5c00 — case-insensitive linked-list string search
// Analysis: re/analysis/sprite_gate_c3/0x004c5c00.md (C2 drift-promote)
// Signature: void* (int list_head_ptr, const char* key)
// Returns ptr to (node-8) on match, or NULL.
// ---------------------------------------------------------------------------
using FUN_004c5c00_t = void* (__cdecl*)(std::int32_t, const char*);
static FUN_004c5c00_t const s_FUN_004c5c00 =
    reinterpret_cast<FUN_004c5c00_t>(0x004c5c00);

// ---------------------------------------------------------------------------
// SpriteLookupTableA  --  0x0040bb70
//
// Original: FUN_0040bb70 (20 bytes, 0x0040bb70..0x0040bb84)
// Signature: void* (const char* key)
//   Calls FUN_004c5c00(DAT_0063b900, key).
//   DAT_0063b900 = sprite linked-list head A (global at 0x0063b900).
// Returns: matched node pointer or NULL.
//
// Ghidra decomp (re/analysis/frontend_promote_menus_b/0040bb70.md):
//   FUN_004c5c00(DAT_0063b900, param_1)
// ref: re/analysis/frontend_promote_menus_b/0040bb70.md
// ref: re/analysis/sprite_gate_c3/0x004c5c00.md
// ---------------------------------------------------------------------------

// 0x0040bb70
extern "C" __declspec(dllexport) void* __cdecl SpriteLookupTableA(const char* key) {
    // 0x0040bb74: DAT_0063b900 — sprite table A linked-list head
    const std::int32_t tableA = *reinterpret_cast<const std::int32_t*>(0x0063b900u);
    return s_FUN_004c5c00(tableA, key);
}

RH_ScopedInstall(SpriteLookupTableA, 0x0040bb70);

// ---------------------------------------------------------------------------
// SpriteLookupTableB  --  0x0040bb90
//
// Original: FUN_0040bb90 (20 bytes, 0x0040bb90..0x0040bba4)
// Signature: void* (const char* key)
//   Calls FUN_004c5c00(DAT_0063b904, key).
//   DAT_0063b904 = sprite linked-list head B (global at 0x0063b904; offset +4 from A).
// Returns: matched node pointer or NULL.
//
// Ghidra decomp (re/analysis/frontend_promote_menus_b/0040bb90.md):
//   FUN_004c5c00(DAT_0063b904, param_1)
// ref: re/analysis/frontend_promote_menus_b/0040bb90.md
// ref: re/analysis/sprite_gate_c3/0x004c5c00.md
// ---------------------------------------------------------------------------

// 0x0040bb90
extern "C" __declspec(dllexport) void* __cdecl SpriteLookupTableB(const char* key) {
    // 0x0040bb94: DAT_0063b904 — sprite table B linked-list head (offset +4 from A)
    const std::int32_t tableB = *reinterpret_cast<const std::int32_t*>(0x0063b904u);
    return s_FUN_004c5c00(tableB, key);
}

RH_ScopedInstall(SpriteLookupTableB, 0x0040bb90);

// ---------------------------------------------------------------------------
// SpriteSlotGate  --  0x0042ee00
//
// Original: FUN_0042ee00 (59 bytes, 0x0042ee00..0x0042ee3b)
// Signature: void* (int slot)
//   For slot 0, 1, or 2: tail-calls FUN_0040bb50() (the DAT_0063b8fc variant).
//   For any other slot: returns 0 (null pointer).
//
// Note: FUN_0040bb50 receives no explicit argument visible in Ghidra decomp
// for all three branches — the string key arrives via a non-standard mechanism
// (likely a register or caller-set global not captured by the decompiler).
// We call the original FUN_0040bb50 directly to preserve exact ABI semantics.
//
// Ghidra decomp (re/analysis/promote_c2_frontend_menus/0x0042ee00.md):
//   if (slot == 0) return FUN_0040bb50();
//   if (slot == 1) return FUN_0040bb50();
//   if (slot == 2) return FUN_0040bb50();
//   return 0;
// ref: re/analysis/promote_c2_frontend_menus/0x0042ee00.md
// ref: re/analysis/hud_frontend/0x0040bb50.md
// ---------------------------------------------------------------------------

// Callee: FUN_0040bb50 — sprite lookup via DAT_0063b8fc
// Analysis: re/analysis/hud_frontend/0x0040bb50.md (C2 drift-promote)
// Called with no visible argument (string arrives via hidden register from caller).
// Reimpl calls through original to preserve exact calling convention.
using FUN_0040bb50_t = void* (__cdecl*)();
static FUN_0040bb50_t const s_FUN_0040bb50 =
    reinterpret_cast<FUN_0040bb50_t>(0x0040bb50);

// 0x0042ee00
extern "C" __declspec(dllexport) void* __cdecl SpriteSlotGate(int slot) {
    // 0x0042ee07: comparison for slot==0
    if (slot == 0) return s_FUN_0040bb50();
    // 0x0042ee12: comparison for slot==1
    if (slot == 1) return s_FUN_0040bb50();
    // 0x0042ee1d: comparison for slot==2
    if (slot == 2) return s_FUN_0040bb50();
    // 0x0042ee38: default — return 0
    return nullptr;
}

RH_ScopedInstall(SpriteSlotGate, 0x0042ee00);

// ---------------------------------------------------------------------------
// HudSlotTypePlayer0  --  0x00430a10
//
// Original: FUN_00430a10 (40 bytes, 0x00430a10..0x00430a38)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 0 given the current game mode:
//   Mode 2         → 0
//   Mode 3, 4, 5   → 1
//   Mode 6, 7, 8, 9→ 3
//   Mode 10        → 11 (0xb)
//   Other          → 0 (default/implicit)
//
// ref: re/analysis/hud_frontend_d4/0x00430a10.md
// ---------------------------------------------------------------------------

// 0x00430a10
extern "C" __declspec(dllexport) int __cdecl HudSlotTypePlayer0() {
    // 0x00430a17: read DAT_0067e9fc; switch on (value - 2)
    // Default path: the switch is on (mode-2); default falls through returning (mode-2).
    // Observed: mode 0 → 0xFFFFFFFE (-2) = 0-2; mode default → mode-2.
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(0x0067e9fcu);
    switch (mode) {
        case 2:  return 0;   // 0x00430a1f
        case 3:              // 0x00430a23
        case 4:              // 0x00430a27
        case 5:  return 1;   // 0x00430a2b
        case 6:              // 0x00430a2f
        case 7:              // 0x00430a2f
        case 8:              // 0x00430a2f
        case 9:  return 3;   // 0x00430a2f
        case 10: return 11;  // 0x00430a33 (0xb)
        default: return mode - 2;  // 0x00430a36: default — (mode-2) computed by switch dispatch
    }
}

RH_ScopedInstall(HudSlotTypePlayer0, 0x00430a10);

// ---------------------------------------------------------------------------
// HudSlotTypePlayer1  --  0x00430a60
//
// Original: FUN_00430a60 (28 bytes, 0x00430a60..0x00430a7c)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 1:
//   Mode 2, 6, 7, 8, 9, 10 → 0
//   Mode 3, 4, 5            → 2
//   Other                   → 0 (default)
//
// ref: re/analysis/hud_frontend_d4/0x00430a60.md
// ---------------------------------------------------------------------------

// 0x00430a60
extern "C" __declspec(dllexport) int __cdecl HudSlotTypePlayer1() {
    // 0x00430a67: read DAT_0067e9fc; switch on (value - 2)
    // Default (out-of-range modes): returns (mode-2) per switch dispatch arithmetic.
    // Observed: mode 0 → 0xFFFFFFFE (-2) = 0-2.
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(0x0067e9fcu);
    switch (mode) {
        case 3:              // 0x00430a6e
        case 4:              // 0x00430a72
        case 5:  return 2;   // 0x00430a72
        case 2:  return 0;   // 0x00430a76
        case 6:  return 0;   // 0x00430a76
        case 7:  return 0;   // 0x00430a76
        case 8:  return 0;   // 0x00430a76
        case 9:  return 0;   // 0x00430a76
        case 10: return 0;   // 0x00430a76
        default: return mode - 2;  // 0x00430a79: default — (mode-2)
    }
}

RH_ScopedInstall(HudSlotTypePlayer1, 0x00430a60);

// ---------------------------------------------------------------------------
// HudSlotTypePlayer2  --  0x00430ab0
//
// Original: FUN_00430ab0 (28 bytes, 0x00430ab0..0x00430acc)
// Signature: int ()   — no arguments; reads DAT_0067e9fc (game-mode global).
// Returns the slot-type code for player slot 2:
//   Mode 2, 6, 7, 8, 9, 10 → 0
//   Mode 3, 4, 5            → 5
//   Other                   → 0 (default)
//
// ref: re/analysis/hud_frontend_d4/0x00430ab0.md
// ---------------------------------------------------------------------------

// 0x00430ab0
extern "C" __declspec(dllexport) int __cdecl HudSlotTypePlayer2() {
    // 0x00430ab7: read DAT_0067e9fc; switch on (value - 2)
    // Default (out-of-range modes): returns (mode-2) per switch dispatch arithmetic.
    // Observed: mode 0 → 0xFFFFFFFE (-2) = 0-2.
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(0x0067e9fcu);
    switch (mode) {
        case 3:              // 0x00430abe
        case 4:              // 0x00430ac2
        case 5:  return 5;   // 0x00430ac2
        case 2:  return 0;   // 0x00430ac6
        case 6:  return 0;   // 0x00430ac6
        case 7:  return 0;   // 0x00430ac6
        case 8:  return 0;   // 0x00430ac6
        case 9:  return 0;   // 0x00430ac6
        case 10: return 0;   // 0x00430ac6
        default: return mode - 2;  // 0x00430ac9: default — (mode-2)
    }
}

RH_ScopedInstall(HudSlotTypePlayer2, 0x00430ab0);
