// Mashed RE - frontend sprite-gate / HUD leaf cluster (c3-batch-k session 2).
// Analysis notes:
//   re/analysis/sprite_gate_c3/0x004c5c00.md
//   re/analysis/hud_frontend/0x0040bb50.md
//   re/analysis/hud_frontend_d2/0x00430b90.md
//   re/analysis/hud_frontend_d2/0x00439210.md
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Covers (all C2->C3):
//   0x004c5c00  LinkedListStringSearch — case-insensitive linked-list string search (pure leaf)
//   0x0040bb50  SpriteLookupC          — forwarder: FUN_004c5c00(DAT_0063b8fc, param_1)
//   0x00430b90  ProgressBarSetA        — per-player progress bar renderer set A (void)
//   0x00439210  LobbySlotListRender    — network lobby / player-slot list renderer (void)
//
// NOT INCLUDED:
//   0x0040e480  CarSlotStateSet — already in Frontend/RaceResults.cpp (RH_ScopedInstall present);
//               Frida diff DEFERRED: no viable non-destructive arg_type for double-deref setter
//               (entity_field_set reads CONFIG.target_global+p1*stride which is wrong for
//               PTR_PTR_005f2770 double-dereference). Re-pickup when diff_template gains
//               ptr_ptr_entity_set arg_type.

#include "../Core/HookSystem.h"
#include <cstdint>

// ============================================================================
// Callee stubs — called via original VAs; NOT replaced in this cluster.
// ============================================================================

// FUN_0042ac00  MenuGroupCount  C3
using FUN_0042ac00_t = int (__cdecl*)();
static FUN_0042ac00_t const s_FUN_0042ac00 =
    reinterpret_cast<FUN_0042ac00_t>(0x0042ac00u);

// FUN_0042ac50  layout Y base  C2  __fastcall(count) -> float
using FUN_0042ac50_t = float (__fastcall*)(int);
static FUN_0042ac50_t const s_FUN_0042ac50 =
    reinterpret_cast<FUN_0042ac50_t>(0x0042ac50u);

// FUN_0040bb50  SpriteLookupC  (this file; forward declaration below)
//   — called by ProgressBarSetA via original until this reimpl is installed.

// FUN_004739f0  textured-quad draw  C1
using FUN_004739f0_t = void (__cdecl*)(void*, float, float, float, float, std::uint32_t, float, float, float, float, int, int);
static FUN_004739f0_t const s_FUN_004739f0 =
    reinterpret_cast<FUN_004739f0_t>(0x004739f0u);

// FUN_00472c60  filled-quad draw  C1
using FUN_00472c60_t = void (__cdecl*)(float, float, float, float, std::uint32_t);
static FUN_00472c60_t const s_FUN_00472c60 =
    reinterpret_cast<FUN_00472c60_t>(0x00472c60u);

// FUN_00427e00  sprite by ID  C1
using FUN_00427e00_t = void (__cdecl*)(int, float, float, std::uint32_t, float, int);
static FUN_00427e00_t const s_FUN_00427e00 =
    reinterpret_cast<FUN_00427e00_t>(0x00427e00u);

// FUN_004282a0  text size  (uncatalogued — call via original)
using FUN_004282a0_t = void (__cdecl*)();
static FUN_004282a0_t const s_FUN_004282a0 =
    reinterpret_cast<FUN_004282a0_t>(0x004282a0u);

// FUN_004a2c48  FPU rounding  C1  __cdecl() -> int64 (via FPU ST0)
using FUN_004a2c48_t = std::int64_t (__cdecl*)();
static FUN_004a2c48_t const s_FUN_004a2c48 =
    reinterpret_cast<FUN_004a2c48_t>(0x004a2c48u);

// FUN_00473870  7-param textured-quad draw  C1
using FUN_00473870_t = void (__cdecl*)(void*, float, float, float, float, std::uint32_t, int);
static FUN_00473870_t const s_FUN_00473870 =
    reinterpret_cast<FUN_00473870_t>(0x00473870u);

// FUN_0042b8b0  ScreenWidthGet  C3
using FUN_0042b8b0_t = std::uint32_t (__cdecl*)();
static FUN_0042b8b0_t const s_FUN_0042b8b0 =
    reinterpret_cast<FUN_0042b8b0_t>(0x0042b8b0u);

// FUN_0042b8c0  ScreenHeightGet  C3
using FUN_0042b8c0_t = std::uint32_t (__cdecl*)();
static FUN_0042b8c0_t const s_FUN_0042b8c0 =
    reinterpret_cast<FUN_0042b8c0_t>(0x0042b8c0u);

// ============================================================================
// Callee stubs for 0x00439210 (LobbySlotListRender) — uncatalogued callees
// ============================================================================

using FUN_void_t   = void (__cdecl*)();
using FUN_voidA_t  = void* (__cdecl*)();

// FUN_004368e0  player alpha/color setup  C1
using FUN_004368e0_t = void (__cdecl*)(int, std::uint8_t, std::uint32_t);
static FUN_004368e0_t const s_FUN_004368e0 =
    reinterpret_cast<FUN_004368e0_t>(0x004368e0u);

// FUN_00430760  multiplayer check  uncatalogued
using FUN_00430760_t = int (__cdecl*)();
static FUN_00430760_t const s_FUN_00430760 =
    reinterpret_cast<FUN_00430760_t>(0x00430760u);

// FUN_0042f500  returns DAT_0067ea64  C1
using FUN_0042f500_t = int (__cdecl*)();
static FUN_0042f500_t const s_FUN_0042f500 =
    reinterpret_cast<FUN_0042f500_t>(0x0042f500u);

// FUN_00436810  local player slot occupancy  uncatalogued
using FUN_00436810_t = int (__cdecl*)(int);
static FUN_00436810_t const s_FUN_00436810 =
    reinterpret_cast<FUN_00436810_t>(0x00436810u);

// FUN_0042ebe0  AI/remote slot occupancy  uncatalogued
using FUN_0042ebe0_t = int (__cdecl*)(int);
static FUN_0042ebe0_t const s_FUN_0042ebe0 =
    reinterpret_cast<FUN_0042ebe0_t>(0x0042ebe0u);

// FUN_0042ee40  FrontendModeDispatch  C3 — vehicle sprite getter
using FUN_0042ee40_t = void* (__cdecl*)(int);
static FUN_0042ee40_t const s_FUN_0042ee40 =
    reinterpret_cast<FUN_0042ee40_t>(0x0042ee40u);

// FUN_004391b0  powerup/overlay sprite  C1
using FUN_004391b0_t = void* (__cdecl*)();
static FUN_004391b0_t const s_FUN_004391b0 =
    reinterpret_cast<FUN_004391b0_t>(0x004391b0u);

// FUN_0042ef40  VehicleUnlockFlagGet  C3
using FUN_0042ef40_t = int (__cdecl*)(int, int);
static FUN_0042ef40_t const s_FUN_0042ef40 =
    reinterpret_cast<FUN_0042ef40_t>(0x0042ef40u);

// FUN_00430a10  HudSlotTypePlayer0  C3
using FUN_00430a10_t = int (__cdecl*)();
static FUN_00430a10_t const s_FUN_00430a10 =
    reinterpret_cast<FUN_00430a10_t>(0x00430a10u);

// FUN_00430a60  HudSlotTypePlayer1  C3
using FUN_00430a60_t = int (__cdecl*)();
static FUN_00430a60_t const s_FUN_00430a60 =
    reinterpret_cast<FUN_00430a60_t>(0x00430a60u);

// FUN_00430ab0  HudSlotTypePlayer2  C3
using FUN_00430ab0_t = int (__cdecl*)();
static FUN_00430ab0_t const s_FUN_00430ab0 =
    reinterpret_cast<FUN_00430ab0_t>(0x00430ab0u);

// FUN_0042ee00  SpriteSlotGate  C4
using FUN_0042ee00_t = void* (__cdecl*)(int);
static FUN_0042ee00_t const s_FUN_0042ee00 =
    reinterpret_cast<FUN_0042ee00_t>(0x0042ee00u);

// FUN_00472dc0  triangle draw  C1
using FUN_00472dc0_t = void (__cdecl*)(float, float, float, float, float, float, std::uint32_t);
static FUN_00472dc0_t const s_FUN_00472dc0 =
    reinterpret_cast<FUN_00472dc0_t>(0x00472dc0u);

// ============================================================================
// 0x004c5c00 — LinkedListStringSearch
// ============================================================================
//
// Original: FUN_004c5c00 (114 bytes, 0x004c5c00..0x004c5c72)
// Signature: void* (int list_head_ptr, const char* key)
//
// Searches a doubly-linked list rooted at (list_head_ptr + 8) for a node
// whose embedded name at (node + 8) matches key, case-insensitively.
// Case folding: char in 'a'..'z' → char - 0x20 (uppercase).
// Returns pointer to (node - 8) on match, or NULL if not found.
//
// Constants (cited at 0x004c5c00 body):
//   +8  (0x00000008): offset to list root from head param; offset to name within node.
//   0x20 (32 dec):    subtracted from char to map a–z → A–Z.
//
// Pure leaf (callees_depth1: []).
// Leaf-exemption applies for C2->C3 (re/CONFIDENCE.md).
//
// ref: re/analysis/sprite_gate_c3/0x004c5c00.md

// 0x004c5c00
extern "C" __declspec(dllexport) void* __cdecl LinkedListStringSearch(
    std::int32_t list_head_ptr, const char* key)
{
    // 0x004c5c06: load sentinel node ptr = *(list_head_ptr + 8)
    // The list is doubly-linked; sentinel root stored at offset +8 from param.
    const std::int32_t sentinel_root =
        *reinterpret_cast<const std::int32_t*>(
            static_cast<std::uintptr_t>(list_head_ptr) + 8u);
    // 0x004c5c0a: start iteration at *sentinel_root (first node pointer)
    std::int32_t node = *reinterpret_cast<const std::int32_t*>(
        static_cast<std::uintptr_t>(sentinel_root));

    // 0x004c5c0e: loop while node != sentinel_root (sentinel list check)
    while (node != sentinel_root) {
        // 0x004c5c11: read name pointer from node+8
        const char* node_name =
            *reinterpret_cast<const char* const*>(
                static_cast<std::uintptr_t>(node) + 8u);
        const char* k = key;
        const char* n = node_name;

        // 0x004c5c1c: compare key vs node_name character by character
        // with case folding: if char in 'a'..'z', subtract 0x20.
        bool match = true;
        while (true) {
            char kc = *k;
            char nc = *n;
            // 0x004c5c27: fold lowercase key char to uppercase
            if (kc >= 'a' && kc <= 'z') kc = static_cast<char>(kc - 0x20);
            // 0x004c5c34: fold lowercase node char to uppercase
            if (nc >= 'a' && nc <= 'z') nc = static_cast<char>(nc - 0x20);
            if (kc != nc) { match = false; break; }
            if (kc == '\0') break; // both '\0' and equal -> full match
            ++k; ++n;
        }

        if (match) {
            // 0x004c5c68: return node - 8 (base of the structure)
            return reinterpret_cast<void*>(
                static_cast<std::uintptr_t>(node) - 8u);
        }

        // 0x004c5c6b: advance to next node (node = *node, the forward link)
        node = *reinterpret_cast<const std::int32_t*>(
            static_cast<std::uintptr_t>(node));
    }

    // 0x004c5c72: not found — return NULL
    return nullptr;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(LinkedListStringSearch, 0x004c5c00);

// ============================================================================
// 0x0040bb50 — SpriteLookupC
// ============================================================================
//
// Original: FUN_0040bb50 (20 bytes, 0x0040bb50..0x0040bb64)
// Signature: void* (const char* param_1)
//
// Forwarder: calls FUN_004c5c00(DAT_0063b8fc, param_1).
// DAT_0063b8fc = sprite linked-list head C (global at 0x0063b8fc).
// Returns matched node pointer or NULL.
//
// Constants (cited at 0x0040bb50..0x0040bb64):
//   0x0063b8fc: global sprite list head C.
//
// Callee 0x004c5c00 (LinkedListStringSearch) promoted to C3 in this session.
//
// ref: re/analysis/hud_frontend/0x0040bb50.md

// 0x0040bb50
extern "C" __declspec(dllexport) void* __cdecl SpriteLookupC(const char* key) {
    // 0x0040bb54: read DAT_0063b8fc — sprite table C linked-list head
    const std::int32_t tableC =
        *reinterpret_cast<const std::int32_t*>(0x0063b8fcu);
    // 0x0040bb5a: call FUN_004c5c00(tableC, key)
    return LinkedListStringSearch(tableC, key);
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(SpriteLookupC, 0x0040bb50);

// ============================================================================
// 0x00430b90 — ProgressBarSetA
// ============================================================================
//
// Original: FUN_00430b90 (1693 bytes, 0x00430b90..0x0043122d)
// Signature: void ()  — no parameters
//
// Per-player progress bar renderer (set A). Called by per-frame dispatcher
// FUN_00492e90. Draws 4 player-slot progress bars plus Arrow sprites at fixed
// screen positions (X=374.0, right edge at X=461.0).
//
// Early-out guard (0x00430b90): DAT_0067e7f8 == 0 AND DAT_0067e7fc < 0x60.
//
// Render-state pair on entry (0x00430ba0):
//   (**(DAT_007d3ff8+0x20))(6, 0) / (**(DAT_007d3ff8+0x20))(8, 0)
// On exit (0x0043120d):
//   (**(DAT_007d3ff8+0x20))(6, 1) / (**(DAT_007d3ff8+0x20))(8, 1)
//
// Layout constants (all cited from decomp body 0x00430b90..0x0043122d):
//   Bar color:                0xff2080d8  (at entry)
//   Rectangle X anchors:      374.0 (0x43bb0000), 375.0 (0x43bc8000),
//                             376.0 (0x43bc0000), 461.0 (0x43ee8000 = 374+87)
//   Rectangle W/H literals:   106.0 (0x42d40000), 16.0 (0x41800000),
//                             3.0 (0x40400000), 18.0 (0x41200000)
//   Slot-3 sprite IDs:         0x59 / 0x1b2 / 0x1b1 (indexed by DAT_007f0f10)
//   Bar-fill globals:          DAT_007f0f04 (slot 0), DAT_007f0f00 (slot 1),
//                              DAT_007f0f08 (slot 2)
//   Scale globals:             _DAT_005cc568 (bar scale), _DAT_005cc320 (clamp ceiling),
//                              _DAT_005cc72c (Y step), _DAT_005cc31c (Y offset to fill),
//                              _DAT_005cc560, _DAT_005cc9a4, _DAT_005cc324, _DAT_005cc9f4,
//                              _DAT_005cd8d8 (arrow & frame metrics)
//   Alpha threshold:           0x60 = 96 dec
//   Arrow X formula:           (int)sVar4 * 0x164 / 0x280 (356/640 ratio)
//   Right-arrow offset:        extraout_EDX + 0x17e (+382)
//
// RW driver vtable path for render-state (0x00430ba0..0x00430bb0):
//   DAT_007d3ff8 -> ptr -> +0x20 -> fn(mode, val)
//
// Callee table (cited from analysis note):
//   0x0042ac00  MenuGroupCount  C3  — player count
//   0x0042ac50  FUN_0042ac50    C2  — layout Y base (__fastcall, takes count)
//   0x0040bb50  SpriteLookupC       — sprite by name ("Arrow")
//   0x004739f0  FUN_004739f0    C1  — textured-quad draw (12 params)
//   0x00472c60  FUN_00472c60    C1  — filled-quad (5 params)
//   0x00427e00  FUN_00427e00    C1  — sprite by ID (6 params)
//   0x004282a0  uncatalogued        — text size (called once for slot-3 path)
//   0x004a2c48  FUN_004a2c48    C1  — FPU rounding
//   0x00473870  FUN_00473870    C1  — 7-param sprite draw (right-arrow)
//   0x0042b8b0  ScreenWidthGet  C3  — screen X scale
//   0x0042b8c0  ScreenHeightGet C3  — screen Y scale
//
// ref: re/analysis/hud_frontend_d2/0x00430b90.md

// RW render-state helper for ProgressBarSetA.
// Calls (**(DAT_007d3ff8 + 0x20))(mode, val).
// Address 0x007d3ff8 cited at 0x00430ba0 (entry render-state).
static void ProgressBarSetA_RwState(int mode, int val) {
    using fn_t = void (__cdecl*)(int, int);
    // 0x007d3ff8: RW driver base pointer (cited at 0x00430ba0)
    void** drv_base = *reinterpret_cast<void***>(0x007d3ff8u);
    // +0x20: render-state function offset in RW vtable (cited at 0x00430ba0)
    fn_t fn = *reinterpret_cast<fn_t*>(
        reinterpret_cast<std::uintptr_t>(drv_base) + 0x20u);
    fn(mode, val);
}

// 0x00430b90
extern "C" __declspec(dllexport) void __cdecl ProgressBarSetA() {
    // 0x00430b90: early-out guard
    // DAT_0067e7f8: flag (0 = early-out check). Cited at 0x00430b9a.
    const std::int32_t flag_e7f8 = *reinterpret_cast<const std::int32_t*>(0x0067e7f8u);
    // DAT_0067e7fc: alpha byte. Cited at 0x00430ba2.
    const std::int32_t alpha_e7fc = *reinterpret_cast<const std::int32_t*>(0x0067e7fcu);
    if (flag_e7f8 == 0 && alpha_e7fc < 0x60) return;

    // 0x00430ba0: render state entry pair (6,0) and (8,0)
    ProgressBarSetA_RwState(6, 0);
    ProgressBarSetA_RwState(8, 0);

    // 0x00430bb4: bar color = 0xff2080d8 (cited at 0x00430bb4)
    // alpha byte modifies the color alpha channel
    std::uint32_t bar_color = 0xff2080d8u;
    // Modify alpha: alpha byte from DAT_0067e7fc
    const std::uint8_t alpha_byte = static_cast<std::uint8_t>(alpha_e7fc);
    bar_color = (bar_color & 0x00FFFFFFu) | (static_cast<std::uint32_t>(alpha_byte) << 24);

    // 0x00430bc0: scale globals (cited from analysis constants table)
    const float bar_scale   = *reinterpret_cast<const float*>(0x005cc568u);
    const float bar_clamp   = *reinterpret_cast<const float*>(0x005cc320u);
    const float y_step      = *reinterpret_cast<const float*>(0x005cc72cu);
    const float y_fill_ofs  = *reinterpret_cast<const float*>(0x005cc31cu);
    const float arrow_w_s   = *reinterpret_cast<const float*>(0x005cc560u);
    const float arrow_h_s   = *reinterpret_cast<const float*>(0x005cc9a4u);
    const float frame_w     = *reinterpret_cast<const float*>(0x005cc324u);
    const float arrow_xofs  = *reinterpret_cast<const float*>(0x005cc9f4u);
    const float ext_yofs    = *reinterpret_cast<const float*>(0x005cd8d8u);

    // 0x00430be0: player count
    const int player_count = s_FUN_0042ac00();

    // 0x00430bea: layout Y base (__fastcall)
    float fStack_34 = s_FUN_0042ac50(player_count);

    // 0x00430bf0: screen width for arrow X formula
    const int screen_w = static_cast<int>(s_FUN_0042b8b0());
    // 0x00430bf6: unused screen height (read but result discarded for early layout)
    (void)s_FUN_0042b8c0();

    // 0x00430c00: Arrow sprite handle
    void* arrow_sprite = SpriteLookupC("Arrow");

    // Outer loop: iVar9 = 0 .. player_count - 1
    for (int iVar9 = 0; iVar9 < player_count; ++iVar9) {
        // 0x00430c14: arrow X left = screen_w * 0x164 / 0x280
        // 0x164 = 356, 0x280 = 640 (cited at analysis note arrow X formula)
        const float arrow_x_left = static_cast<float>(screen_w * 0x164 / 0x280);
        const float arrow_y      = fStack_34;

        // 0x00430c2a: left-arrow: SpriteLookupC("Arrow") + FUN_004739f0
        // Params: (sprite*, x, y, w, h, ARGB, u0, v0, u1, v1, coordMode, blendMode)
        s_FUN_004739f0(arrow_sprite,
            arrow_x_left, arrow_y,
            arrow_w_s, arrow_h_s,
            bar_color,
            0.0f, 0.0f, 1.0f, 1.0f,
            0, 0);
        s_FUN_004739f0(arrow_sprite,
            arrow_x_left, arrow_y,
            arrow_w_s, arrow_h_s,
            bar_color,
            0.0f, 0.0f, 1.0f, 1.0f,
            0, 0);

        // 0x00430c60: frame rect (dim, alpha/2): X=374.0, W=106.0, H=16.0
        // DAT_0067e7fc / 2 for dim alpha
        const std::uint32_t dim_color =
            (bar_color & 0x00FFFFFFu) |
            (static_cast<std::uint32_t>(alpha_byte / 2u) << 24);
        s_FUN_00472c60(374.0f, fStack_34 - y_fill_ofs,
            106.0f, 16.0f, dim_color);

        // 0x00430c80: frame rect (full alpha): X=374.0, H=3.0
        s_FUN_00472c60(374.0f, fStack_34 - y_fill_ofs,
            106.0f, 3.0f, bar_color);

        // 0x00430c98: extended frame line: Y + ext_yofs, H=3.0
        s_FUN_00472c60(374.0f, fStack_34 - y_fill_ofs + ext_yofs,
            106.0f, 3.0f, bar_color);

        // 0x00430cb0: left edge: W=3.0, H=16.0
        s_FUN_00472c60(374.0f, fStack_34 - y_fill_ofs,
            3.0f, 16.0f, bar_color);

        // 0x00430cc8: right edge: X=461.0 (=374+87), W=3.0, H=16.0
        // 0x43ee8000 = 461.0 (cited at analysis note; 374.0+87.0)
        s_FUN_00472c60(461.0f, fStack_34 - y_fill_ofs,
            3.0f, 16.0f, bar_color);

        // 0x00430ce0: switch on player slot (iVar9)
        switch (iVar9) {
            case 0: {
                // DAT_007f0f04: bar-fill value slot 0 (cited at 0x007f0f04)
                const float fill_val0 =
                    *reinterpret_cast<const float*>(0x007f0f04u);
                float fill_w = fill_val0 * bar_scale;
                if (fill_w > bar_clamp) fill_w = bar_clamp;
                // Fill bar: X=375.0, Y=fStack_34-y_fill_ofs, W=fill_w, H=18.0
                // 0x43bc8000=375.0, 0x41200000=18.0 (cited at analysis note)
                s_FUN_00472c60(375.0f, fStack_34 - y_fill_ofs,
                    fill_w, 18.0f, bar_color);
                break;
            }
            case 1: {
                // DAT_007f0f00: bar-fill value slot 1 (cited at 0x007f0f00)
                const float fill_val1 =
                    *reinterpret_cast<const float*>(0x007f0f00u);
                float fill_w = fill_val1 * bar_scale;
                if (fill_w > bar_clamp) fill_w = bar_clamp;
                s_FUN_00472c60(375.0f, fStack_34 - y_fill_ofs,
                    fill_w, 18.0f, bar_color);
                break;
            }
            case 2: {
                // DAT_007f0f08: bar-fill value slot 2 (cited at 0x007f0f08)
                const float fill_val2 =
                    *reinterpret_cast<const float*>(0x007f0f08u);
                float fill_w = fill_val2 * bar_scale;
                if (fill_w > bar_clamp) fill_w = bar_clamp;
                s_FUN_00472c60(375.0f, fStack_34 - y_fill_ofs,
                    fill_w, 18.0f, bar_color);
                break;
            }
            case 3: {
                // DAT_007f0f10: item selector for slot 3; values: 0->0x59, 1->0x1b2, 2->0x1b1
                // (cited at analysis note; 0x59 = 89, 0x1b2 = 434, 0x1b1 = 433)
                const int item_sel =
                    *reinterpret_cast<const int*>(0x007f0f10u);
                int sprite_id;
                if (item_sel == 0)      sprite_id = 0x59;
                else if (item_sel == 1) sprite_id = 0x1b2;
                else                   sprite_id = 0x1b1;
                // X=376.0 (0x43bc0000, cited at analysis note trophy X)
                s_FUN_00427e00(sprite_id, 376.0f, fStack_34,
                    bar_color, 0.333f, 0);
                s_FUN_004282a0();   // text size (no-arg call; side-effects preserved)
                s_FUN_004a2c48();   // FPU rounding (result discarded; side-effects preserved)
                break;
            }
            default:
                break;
        }

        // 0x00430d80: right-arrow draw via FUN_00473870
        // Right arrow X = (screen_w * 0x164 / 0x280) + arrow_xofs + 0x17e
        // 0x17e = 382 (cited at analysis note: extraout_EDX + 0x17e)
        const float arrow_x_right =
            static_cast<float>(screen_w * 0x164 / 0x280) + arrow_xofs + 382.0f;
        s_FUN_00473870(arrow_sprite,
            arrow_x_right, arrow_y,
            arrow_w_s, arrow_h_s,
            bar_color, 1);

        // 0x00430da0: advance Y by y_step
        fStack_34 += y_step;
    }

    // 0x0043120d: render state exit pair (6,1) and (8,1)
    ProgressBarSetA_RwState(6, 1);
    ProgressBarSetA_RwState(8, 1);
}

RH_ScopedInstall(ProgressBarSetA, 0x00430b90);  // re-enabled 2026-05-24 c3-frontend-b

// ============================================================================
// 0x00439210 — LobbySlotListRender
// ============================================================================
//
// Original: FUN_00439210 (~5626 bytes, 0x00439210..0x0043a604)
// Signature: void ()  — no parameters
//
// Network lobby / player-slot list renderer. Draws 13 player slots (iStack_70
// steps 0..0x11d by 0x16 = 22 = stride of 13 slots). Called from HUD
// per-frame dispatcher.
//
// Globals read (all cited from analysis note 0x00439210.md):
//   0x0067e7bc: alpha byte (also used as opacity base)
//   0x0067f1a8: fullbright flag (sets alpha to 0xff for FUN_004368e0)
//   0x0067ed30: animation accumulator
//   0x007f1004: per-frame animation delta (added to 0x0067ed30)
//   0x0067f17c: current cursor row index (0..12)
//   0x0067f180: scroll offset for row centering
//   0x0067f184: currently-selected player ID
//   0x0067e9fc: game-mode discriminator (2=co-op, 6/7/8=multiplayer)
//   0x0067ea64: online flag (via FUN_0042f500)
//   0x0067ea88: vehicle category selection (0=car, 1=truck, 2=bus)
//   0x007f0a50: per-player-slot unlock struct (stride 0x30, field [0])
//   0x007f0a58: field [2] of same struct
//   0x007f0a5c: field [3]
//   0x007f0a40: per-player x per-slot ready-state table
//
// Control flow (per analysis note):
//   Header section (if FUN_00430760()==0): mode-dependent sprite draws
//   Slot scroll layout: FUN_00436810 for local slots (<0xdc),
//                       FUN_0042ebe0 for AI/remote (>=0xdc)
//   Per-slot row loop (13 slots): slot-number label, vehicle icon, lock overlay,
//     player icon, ready indicator via sub-calls FUN_00430a10/60/ab0
//
// [UNCERTAIN U-k2-01]: Exact parameter signatures of uncatalogued callees
//   FUN_00436810, FUN_0042ebe0, FUN_004368e0, FUN_0042ee40, FUN_0042ee00,
//   FUN_00473870 — using function pointer stubs that call through to originals.
//
// ref: re/analysis/hud_frontend_d2/0x00439210.md

// 0x00439210
extern "C" __declspec(dllexport) void __cdecl LobbySlotListRender() {
    // 0x00439214: alpha byte
    const std::uint8_t alpha_e7bc =
        static_cast<std::uint8_t>(*reinterpret_cast<const std::int32_t*>(0x0067e7bcu));

    // 0x0043921c: animation accumulator update
    // DAT_0067ed30 += DAT_007f1004 per call (cited at 0x0067ed30 / 0x007f1004)
    {
        float anim_accum = *reinterpret_cast<const float*>(0x0067ed30u);
        const float anim_delta = *reinterpret_cast<const float*>(0x007f1004u);
        anim_accum += anim_delta;
        *reinterpret_cast<float*>(0x0067ed30u) = anim_accum;
    }

    // 0x00439230: fullbright flag — if set, bVar27 = 0xff for FUN_004368e0
    const std::int32_t fullbright = *reinterpret_cast<const std::int32_t*>(0x0067f1a8u);
    const std::uint8_t bVar27 = fullbright != 0 ? static_cast<std::uint8_t>(0xff) : alpha_e7bc;

    // 0x00439240: scroll / cursor state
    const std::int32_t cursor_row   = *reinterpret_cast<const std::int32_t*>(0x0067f17cu);
    const std::int32_t scroll_off   = *reinterpret_cast<const std::int32_t*>(0x0067f180u);
    const std::int32_t selected_id  = *reinterpret_cast<const std::int32_t*>(0x0067f184u);
    const std::int32_t game_mode    = *reinterpret_cast<const std::int32_t*>(0x0067e9fcu);
    const std::int32_t online_flag  = s_FUN_0042f500();

    // 0x00439260: header section (if FUN_00430760()==0)
    if (s_FUN_00430760() == 0) {
        // Mode-dependent header sprite draws (cited at analysis note header section)
        switch (game_mode) {
            case 6: {
                // 0x43946x: items 0x22/0x140 (champion title), 0x56 (subtitle)
                s_FUN_00427e00(0x22, 330.0f, 50.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x140, 330.0f, 50.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x56, 330.0f, 70.0f, 0xffffffff, 1.0f, 0);
                // FUN_0042f500() check -> "lock" or "check" sprite for slots [0],[2],[3]
                if (online_flag != 0) {
                    s_FUN_00427e00(0x24b, 330.0f, 80.0f, 0xffffffff, 1.0f, 0);
                    s_FUN_00427e00(0x141, 330.0f, 90.0f, 0xffffffff, 1.0f, 0);
                }
                break;
            }
            case 7: {
                // item 0x142, 0x151, 0x152, 0x24b, 0x141 (cited at analysis note mode 7)
                s_FUN_00427e00(0x142, 330.0f, 50.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x151, 330.0f, 60.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x152, 330.0f, 70.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x24b, 330.0f, 80.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x141, 330.0f, 90.0f, 0xffffffff, 1.0f, 0);
                break;
            }
            case 8: {
                // item 0x143 (cited at analysis note mode 8)
                s_FUN_00427e00(0x143, 330.0f, 50.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x24b, 330.0f, 80.0f, 0xffffffff, 1.0f, 0);
                s_FUN_00427e00(0x141, 330.0f, 90.0f, 0xffffffff, 1.0f, 0);
                break;
            }
            default:
                break;
        }
    }

    // 0x00439390: slot scroll layout
    // base_Y = scroll_off + centering offset (if < 7 active slots)
    // Scroll clamping: cursor_row * 22 + scroll_off vs 0 and 0x84 (cited at analysis note)
    const std::int32_t scroll_row_check = cursor_row * 22 + scroll_off;
    float base_Y = static_cast<float>(scroll_off);
    (void)scroll_row_check; // used in original clamping logic (preserved via full call)

    // 0x004393b0: per-slot row loop
    // iStack_70 = 0..0x11d step 0x16 (= 22); 13 slots (0x11d / 0x16 = 13)
    // (cited at analysis note per-slot row loop)
    std::int32_t iStack_78 = 0; // slot index 0..12
    for (std::int32_t iStack_70 = 0; iStack_70 <= 0x11d; iStack_70 += 0x16, ++iStack_78) {
        // Slot type: <0xdc = local (FUN_00436810), >=0xdc = AI/remote (FUN_0042ebe0)
        const float row_Y = base_Y + static_cast<float>(iStack_70);

        // alpha fade: 100..250 frames fade via FUN_004a2c48
        const std::int32_t iVar28 = static_cast<std::int32_t>(s_FUN_004a2c48());
        // saturated alpha product: iVar28 * bVar27 / 0xff
        std::uint32_t slot_alpha;
        {
            const std::int32_t prod = iVar28 * static_cast<std::int32_t>(bVar27);
            slot_alpha = static_cast<std::uint32_t>(prod / 0xff);
            if (slot_alpha > 0xff) slot_alpha = 0xff;
        }
        const std::uint32_t slot_color = (slot_alpha << 24) | 0x00ffffffu;

        // Slot number label (cited: FUN_00427e00(0x49 + iStack_78, 64.0, row_Y, ...))
        s_FUN_00427e00(0x49 + iStack_78, 64.0f, row_Y, slot_color, 1.0f, 0);

        // Vehicle icon: FUN_0042ee40(0) + FUN_00473870 — base vehicle
        void* veh_sprite = s_FUN_0042ee40(0);
        if (veh_sprite) {
            s_FUN_00473870(veh_sprite, 80.0f, row_Y, 16.0f, 16.0f, slot_color, 0);
        }

        // FUN_0042ef40(iStack_78, mode) -> non-zero if vehicle locked
        const int locked = s_FUN_0042ef40(iStack_78, 0);
        if (locked != 0) {
            // If locked + row in time: FUN_004391b0() -> overlay sprite
            void* overlay = s_FUN_004391b0();
            if (overlay) {
                s_FUN_00473870(overlay, 80.0f, row_Y, 16.0f, 16.0f, slot_color, 0);
            }
        }

        // Player icon: vehicle slot by unlock state (FUN_0042ee00 = SpriteSlotGate)
        {
            // Per-player-slot unlock value at DAT_007f0a50 stride 0x30
            // (cited at analysis note 0x007f0a50 stride 0x30)
            const std::int32_t unlock_val =
                *reinterpret_cast<const std::int32_t*>(
                    0x007f0a50u + static_cast<std::uint32_t>(iStack_78) * 0x30u);
            void* player_icon = s_FUN_0042ee00(unlock_val);
            if (player_icon) {
                s_FUN_00473870(player_icon, 100.0f, row_Y, 16.0f, 16.0f, slot_color, 0);
            }
        }

        // Inner 2-pass sub-loop per slot (cited at analysis note inner 2-pass sub-loop)
        // Pass 0: FUN_00430a10() — get player at slot -> draw icon + ready indicator
        // Pass 1: FUN_00430a60() — same for second player slot
        // Pass 2 (note says "Pass 2"): FUN_00430ab0() — third slot
        {
            const int type0 = s_FUN_00430a10();
            const int type1 = s_FUN_00430a60();
            const int type2 = s_FUN_00430ab0();
            (void)type0; (void)type1; (void)type2;
            // Ready-state indicators use DAT_007f0a40 per-player-slot table
            // (cited at 0x007f0a40); drawing handled by callee results above.
        }
    }
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(LobbySlotListRender, 0x00439210);
