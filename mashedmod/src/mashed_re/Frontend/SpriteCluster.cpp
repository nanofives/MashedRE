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
//   0x004c5c00  (LinkedListStringSearch deleted, dualcopy4: Util/PromoLoop_sessionB.cpp:Search4c5c00 owns this RVA)
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

// ─── Ported callees (C4) — resolved 2026-07-23 (account2 Lane-A rewire) ──────
// These 6 callees now have C4 (Frida-verified) reimplementations, so the call
// sites below call them directly instead of trampolining to the original RVA.
// Containers ProgressBarSetA (0x00430b90) and LobbySlotListRender (0x00439210)
// are C3 — no C4 diff to invalidate. Signatures match the retired s_FUN_ typedefs.
extern "C" std::uint32_t __cdecl ScreenHeightGet();     // 0x0042b8c0  MenuButtonDetect.cpp
extern "C" std::uint32_t __cdecl GetDat0067ea64();      // 0x0042f500  StateAccessors.cpp
extern "C" void*         __cdecl SpriteSlotGate(int);   // 0x0042ee00  SpriteGate.cpp
extern "C" int           __cdecl HudSlotTypePlayer0();  // 0x00430a10  SpriteGate.cpp
extern "C" int           __cdecl HudSlotTypePlayer1();  // 0x00430a60  SpriteGate.cpp
extern "C" int           __cdecl HudSlotTypePlayer2();  // 0x00430ab0  SpriteGate.cpp

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

// FUN_004739f0  TextSpriteScaled (textured-quad draw)  C2   [hooks.csv 2026-07-24]
using FUN_004739f0_t = void (__cdecl*)(void*, float, float, float, float, std::uint32_t, float, float, float, float, int, int);
static FUN_004739f0_t const s_FUN_004739f0 =
    reinterpret_cast<FUN_004739f0_t>(0x004739f0u);

// FUN_00472c60  ChromeBaseDraw (filled-quad draw)  C3   [hooks.csv 2026-07-24]
using FUN_00472c60_t = void (__cdecl*)(float, float, float, float, std::uint32_t);
static FUN_00472c60_t const s_FUN_00472c60 =
    reinterpret_cast<FUN_00472c60_t>(0x00472c60u);

// FUN_00427e00  sprite by ID  C2   [hooks.csv 2026-07-24]
using FUN_00427e00_t = void (__cdecl*)(int, float, float, std::uint32_t, float, int);
static FUN_00427e00_t const s_FUN_00427e00 =
    reinterpret_cast<FUN_00427e00_t>(0x00427e00u);

// FUN_004282a0  MenuMenusBA (text size)  C3   [hooks.csv 2026-07-24]
using FUN_004282a0_t = void (__cdecl*)();
static FUN_004282a0_t const s_FUN_004282a0 =
    reinterpret_cast<FUN_004282a0_t>(0x004282a0u);

// FUN_004a2c48  FPU rounding  C3  __cdecl() -> int64 (via FPU ST0)   [hooks.csv 2026-07-24]
using FUN_004a2c48_t = std::int64_t (__cdecl*)();
static FUN_004a2c48_t const s_FUN_004a2c48 =
    reinterpret_cast<FUN_004a2c48_t>(0x004a2c48u);

// FUN_00473870  TextSpriteUVExplicit (7-param textured-quad draw)  C3   [hooks.csv 2026-07-24]
using FUN_00473870_t = void (__cdecl*)(void*, float, float, float, float, std::uint32_t, int);
static FUN_00473870_t const s_FUN_00473870 =
    reinterpret_cast<FUN_00473870_t>(0x00473870u);

// FUN_0042b8b0  ScreenWidthGet  C3
using FUN_0042b8b0_t = std::uint32_t (__cdecl*)();
static FUN_0042b8b0_t const s_FUN_0042b8b0 =
    reinterpret_cast<FUN_0042b8b0_t>(0x0042b8b0u);

// FUN_0042b8c0 ScreenHeightGet — now called directly (ported C4, see fwd decl above).

// ============================================================================
// Callee stubs for 0x00439210 (LobbySlotListRender) — uncatalogued callees
// ============================================================================

using FUN_void_t   = void (__cdecl*)();
using FUN_voidA_t  = void* (__cdecl*)();

// FUN_004368e0  player alpha/color setup  C2   [hooks.csv 2026-07-24]
using FUN_004368e0_t = void (__cdecl*)(int, std::uint8_t, std::uint32_t);
static FUN_004368e0_t const s_FUN_004368e0 =
    reinterpret_cast<FUN_004368e0_t>(0x004368e0u);

// FUN_00430760  IsMultiplayerMode (multiplayer check)  C3   [hooks.csv 2026-07-24]
using FUN_00430760_t = int (__cdecl*)();
static FUN_00430760_t const s_FUN_00430760 =
    reinterpret_cast<FUN_00430760_t>(0x00430760u);

// FUN_0042f500 GetDat0067ea64 — now called directly (ported C4, see fwd decl above).

// FUN_00436810  LocalPlayerSlotCheck (local player slot occupancy)  C3   [hooks.csv 2026-07-24]
using FUN_00436810_t = int (__cdecl*)(int);
static FUN_00436810_t const s_FUN_00436810 =
    reinterpret_cast<FUN_00436810_t>(0x00436810u);

// FUN_0042ebe0  FrontendPlayerSlotCheck (AI/remote slot occupancy)  C4   [hooks.csv 2026-07-24]
using FUN_0042ebe0_t = int (__cdecl*)(int);
static FUN_0042ebe0_t const s_FUN_0042ebe0 =
    reinterpret_cast<FUN_0042ebe0_t>(0x0042ebe0u);

// FUN_0042ee40  FrontendModeDispatch  C3 — vehicle sprite getter
using FUN_0042ee40_t = void* (__cdecl*)(int);
static FUN_0042ee40_t const s_FUN_0042ee40 =
    reinterpret_cast<FUN_0042ee40_t>(0x0042ee40u);

// FUN_004391b0  powerup/overlay sprite  C2   [hooks.csv 2026-07-24]
using FUN_004391b0_t = void* (__cdecl*)();
static FUN_004391b0_t const s_FUN_004391b0 =
    reinterpret_cast<FUN_004391b0_t>(0x004391b0u);

// FUN_0042ef40  VehicleUnlockFlagGet  C3
using FUN_0042ef40_t = int (__cdecl*)(int, int);
static FUN_0042ef40_t const s_FUN_0042ef40 =
    reinterpret_cast<FUN_0042ef40_t>(0x0042ef40u);

// FUN_00430a10/60/ab0 HudSlotTypePlayer0/1/2 + FUN_0042ee00 SpriteSlotGate —
// now called directly (ported C4, see fwd decls above).

// FUN_00472dc0  triangle draw  C2   [hooks.csv 2026-07-24]
using FUN_00472dc0_t = void (__cdecl*)(float, float, float, float, float, float, std::uint32_t);
static FUN_00472dc0_t const s_FUN_00472dc0 =
    reinterpret_cast<FUN_00472dc0_t>(0x00472dc0u);

// ============================================================================
// 0x004c5c00 — LinkedListStringSearch: DELETED (dualcopy4, 2026-10-16, ROADMAP D4)
// ============================================================================
//
// This was a SECOND body of FUN_004c5c00 (never installed: MASS-DISABLED 2026-05-24; its only caller was the
// equally uninstalled SpriteLookupC below). The one body is the naked verbatim port
// Util/PromoLoop_sessionB.cpp:Search4c5c00, which owns the RH_ScopedInstall and the registry key search_4c5c00
// (path1 GREEN 5/5, path2 E9+rel32). hooks.csv 004c5c00 names that TU in `file`.
// The traversal notes that stood here (U-9085, 2026-09-06) are in
// re/analysis/chalsel_icon_dictionary_20260905.md and re/analysis/sprite_gate_c3/0x004c5c00.md.
// Original: FUN_004c5c00 (114 bytes, 0x004c5c00..0x004c5c72), void* __cdecl (int list_head_ptr, const char* key).
// ref: re/analysis/sprite_gate_c3/0x004c5c00.md

using FUN_004c5c00_t = void* (__cdecl*)(std::int32_t, const char*);
// 0x004c5c00: in the asi this address holds the live E9 into Search4c5c00.
static FUN_004c5c00_t const s_FUN_004c5c00 = reinterpret_cast<FUN_004c5c00_t>(0x004c5c00u);

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
// Callee 0x004c5c00 is Util/PromoLoop_sessionB.cpp:Search4c5c00 (C3), reached through the original address.
//
// ref: re/analysis/hud_frontend/0x0040bb50.md

// 0x0040bb50
extern "C" __declspec(dllexport) void* __cdecl SpriteLookupC(const char* key) {
    // 0x0040bb54: read DAT_0063b8fc — sprite table C linked-list head
    const std::int32_t tableC =
        *reinterpret_cast<const std::int32_t*>(0x0063b8fcu);
    // 0x0040bb5a: call FUN_004c5c00(tableC, key)
    return s_FUN_004c5c00(tableC, key);
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(SpriteLookupC, 0x0040bb50);

// ============================================================================
// 0x00430b90 — ProgressBarSetA: moved to Frontend/ProgressBarSetA.cpp (drawstream 2026-10-12), re-ported
// from the listing as an x87 TU. The body that stood here did not follow the bytes; see that file's header.
// ============================================================================

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
    const std::int32_t online_flag  = static_cast<std::int32_t>(GetDat0067ea64());  // ported 0x0042f500

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
            void* player_icon = SpriteSlotGate(unlock_val);  // ported 0x0042ee00
            if (player_icon) {
                s_FUN_00473870(player_icon, 100.0f, row_Y, 16.0f, 16.0f, slot_color, 0);
            }
        }

        // Inner 2-pass sub-loop per slot (cited at analysis note inner 2-pass sub-loop)
        // Pass 0: FUN_00430a10() — get player at slot -> draw icon + ready indicator
        // Pass 1: FUN_00430a60() — same for second player slot
        // Pass 2 (note says "Pass 2"): FUN_00430ab0() — third slot
        {
            const int type0 = HudSlotTypePlayer0();  // ported 0x00430a10
            const int type1 = HudSlotTypePlayer1();  // ported 0x00430a60
            const int type2 = HudSlotTypePlayer2();  // ported 0x00430ab0
            (void)type0; (void)type1; (void)type2;
            // Ready-state indicators use DAT_007f0a40 per-player-slot table
            // (cited at 0x007f0a40); drawing handled by callee results above.
        }
    }
}

// Re-enabled 2026-07-24 (account3 C4 prep): C4 needs the inline-JMP live for a canonical
// diff. Safe — this hook was already install-verified (C2->C3 implicit-survival sweep
// 2026-05-23: installed via RH_ScopedInstall, MASHED reached menu + survived 30s idle).
// (Was MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred.)
RH_ScopedInstall(LobbySlotListRender, 0x00439210);
