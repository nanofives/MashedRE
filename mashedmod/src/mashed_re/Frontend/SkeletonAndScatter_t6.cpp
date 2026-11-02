// Mashed RE — c3_batch_t session 6: skeleton_prep + scattered frontend cluster.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Candidates in this file (10 attempted; 3 GREEN-promoted, 4 impl-only/deferred,
// 1 RED live-state-blocked, 2 harness-incompatible deferred, 1 refused):
//   0x004274d0  LangIndexSeedFromCli     — GREEN C3 (read_global; 0/10 mismatch)
//   0x00428390  FrontendStateSet         — GREEN C3 (void_setter_observe; 0/10)
//   0x0042fa00  PlayerSlotEdgeAdjust     — GREEN C3 (none; 0/10 mismatch)
//   0x004274e0  LocalizationFileLoad     — moved to Match/MatchS06_004274e0.cpp (MATCH-S06-FU)
//   0x0042bcb0  PlayerNameSpriteDraw     — impl-only; 5-arg complex frame, no arg_type.
//   (0x00431f30 FrontendPageFlagSet was removed 2026-10-16, dupinstall: its one-body-per-RVA
//    copy is FrontendPageIdDispatch in Frontend/MenuLeaves_af6.cpp, the body hooks.csv's
//    evidence cites)
//   0x00403050  PreRaceLoadingScreenDraw — RED: original AVs at quiescent menu
//                                          (DAT_00771964 non-zero stale handle →
//                                          live-state wall); impl matches decomp.
//
// DEFERRED candidates (not implemented):
//   0x004a2b60  vsprintf-style wrapper — variadic; cannot be exposed via Frida
//               NativeFunction (no varargs in mscdecl harness signature). Filed.
//   0x00497b60  player-config dialog populater — DialogProc-style HWND param;
//               1134B win32 dialog code with LoadStringA/LoadBitmapA/SetWindowTextA;
//               not callable from Frida harness (no dialog context at quiescent menu).
//
// REFUSED:
//   0x00423040  BLOCKED per c3_batch_s-s1: callees FUN_00417450 (C1) and
//               FUN_00417530 (C1) — anti-island rule. Verified 2026-05-26.
//
// Callee gates (verified 2026-05-26 against hooks.csv):
//   FUN_004cc230  C2  (PIZ stream open)
//   FUN_004cbd30  C3  (stream read)
//   FUN_004cc160  C3  (stream close)
//   FUN_004c5c00  C3  (linked-list string search)
//   FUN_004b5750  C2  (Im2D quad draw via vtable)
//   FUN_00431d90  C2  (panel-flag mass-clear)
//   FUN_00402fb0  C2  (pulsing sprite spinner)
//   FUN_00428760  C2  (background quad draw)

#include "../Core/HookSystem.h"
#include "../Core/ShadowTrack.h"
#include "../Core/ShadowAB.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Forward declarations for callees (originals; we call into MASHED.exe RVAs).
// ---------------------------------------------------------------------------

// 0x004cc230  RwStreamOpen-equivalent: (type, mode, name) -> handle (uint32*)
//   type: 1=file 2=PIZ 3=mem 4=cb;  mode: 1=read 2=write 3=append
static auto* const s_FUN_004cc230 = reinterpret_cast<
    std::uint32_t*(__cdecl*)(int, int, const void*)>(0x004cc230);

// 0x004cbd30  RwStreamRead-equivalent: (handle, buf, size) -> bytes_read
static auto* const s_FUN_004cbd30 = reinterpret_cast<
    std::uint32_t(__cdecl*)(std::uint32_t*, void*, std::uint32_t)>(0x004cbd30);

// 0x004cc160  RwStreamClose-equivalent: (handle, hint)
static auto* const s_FUN_004cc160 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t*, std::uint32_t)>(0x004cc160);

// 0x004c5c00  Linked-list string search: (collection, name) -> entry*
static auto* const s_FUN_004c5c00 = reinterpret_cast<
    std::uint32_t*(__cdecl*)(std::uint32_t, const void*)>(0x004c5c00);

// 0x004b5750  Im2D quad draw via vtable: (driver, &rect, &xfm, &param4, handle)
static auto* const s_FUN_004b5750 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t, const float*, const float*, const void*, std::uint32_t)>(0x004b5750);

// 0x00402fb0  Pulsing sprite spinner (no args).
static auto* const s_FUN_00402fb0 = reinterpret_cast<void(__cdecl*)()>(0x00402fb0);

// 0x00428760  Background textured-quad: (handle, x1, y1, x2, y2, mode)
//   Args 2..5 passed as 32-bit IEEE-float-bit-patterns (from original).
static auto* const s_FUN_00428760 = reinterpret_cast<
    void(__cdecl*)(std::uint32_t, float, float, float, float, int)>(0x00428760);

// MATCH-S06: the body of 0x004274d0 (LangIndexSeedFromCli) moved to Match/MatchS06_004274d0.cpp,
// which compiles the byte-matched source re/match/s06/004274d0.c.

// MATCH-S06-FU: the body of 0x004274e0 (LocalizationFileLoad) moved to Match/MatchS06_004274e0.cpp, which
// compiles the byte-matched source re/match/s06/004274e0.c behind the default-OFF MASHED_LOC_USA_DAT knob
// (language index 5 opens USA.dat only with the knob; verify/matchs06fu_20261102/PREREG_MATCH-S06-FU.md).
// The shadow A/B wrapper and the install moved with it.

// MATCH-S06: the body of 0x00428390 (FrontendStateSet) moved to Match/MatchS06_00428390.cpp,
// which compiles the byte-matched source re/match/s06/00428390.c.

// ───────────────────────────────────────────────────────────────────────────
// 0x0042fa00  PlayerSlotEdgeAdjust: DELETED (dualcopy4, 2026-10-16, ROADMAP D4)
// ───────────────────────────────────────────────────────────────────────────
//
// This was the .asi's installed body for FUN_0042fa00 (162/163 bytes, 0x0042fa00..0x0042faa2) and a second copy of
// Frontend/MenuNav.cpp:MenuTeamSelectTick (both targets). The two differed from the original exactly where this copy added a
// `*team > 0` guard and a trailing clamp `[0, 2]`: the decompiled original guards `*team != 0` and has no clamp (decomp_pc 0x0042fa00,
// verify/dualcopy4_20261016/decomp_42fa00.txt: `if (pcVar3[-0x4c0] != 0 && *pcVar3 == 0 && *piVar5 != 0) *piVar5 -= 1`). They agree
// for values already in [0, 2] and differ for any other value. MenuTeamSelectTick follows the original, is the one body for both
// targets and now owns the install and the registry key menu_team_select_tick. The key player_slot_edge_adjust is retired.
// ref: re/analysis/frontend_unmapped_a/0x0042fa00.md

// ───────────────────────────────────────────────────────────────────────────
// 0x00403050  PreRaceLoadingScreenDraw
//
// Original: FUN_00403050 (124 bytes, 0x00403050..0x004030cb)
// Signature: void FUN_00403050(void)
//
// Body:
//   if (DAT_00771964 != 0) {
//     FUN_00428760(DAT_00771964, 320.0, 200.0, 480.0, 240.0, 2)
//     vtable = *(DAT_007d3ff8); (vtable + 0x20)(6, 0)
//                                (vtable + 0x20)(8, 0)
//     FUN_00402fb0()
//     (vtable + 0x20)(6, 1)
//     (vtable + 0x20)(8, 1)
//   }
//
// Stack-cookie guard (DAT_00616038 ^ retaddr) — omitted in reimpl; MSVC /GS
// reinserts equivalent if active.
//
// Constants cited:
//   bg_x1 = 320.0   (0x43a00000)         [body, FUN_00428760 arg2]
//   bg_y1 = 200.0   (0x43480000)         [body, arg3]
//   bg_x2 = 480.0   (0x43f00000)         [body, arg4]
//   bg_y2 = 240.0   (0x43700000)         [body, arg5]
//   coord-mode = 2                       [body, arg6]
//   vtable slot offset = 0x20            [render-state setter]
//   render-state pairs: (6,0)(8,0) disable Z; (6,1)(8,1) restore
//
// ref: re/analysis/loading_screen/0x00403050.md
// ───────────────────────────────────────────────────────────────────────────

// Render-state vtable fn ptr (slot 0x20 on RW driver object).
typedef void (__cdecl* RwSetState_t)(int, int);

// 0x00403050
extern "C" __declspec(dllexport) void __cdecl PreRaceLoadingScreenDraw();
static void __cdecl PreRaceLoadingScreenDraw_impl() {
    std::uint32_t bg = *reinterpret_cast<const std::uint32_t*>(0x00771964u);
    if (bg == 0u) {
        return;
    }

    // Background textured quad (320, 200) -> (480, 240), coord-mode 2.
    s_FUN_00428760(bg, 320.0f, 200.0f, 480.0f, 240.0f, 2);

    // Render-state disable: (6, 0), (8, 0).
    std::uintptr_t vtable_obj = *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u);
    RwSetState_t rs_fn = *reinterpret_cast<RwSetState_t*>(vtable_obj + 0x20u);
    rs_fn(6, 0);
    rs_fn(8, 0);

    // Pulsing sprite spinner.
    s_FUN_00402fb0();

    // Render-state restore: (6, 1), (8, 1). Re-read vtable in case state moved.
    vtable_obj = *reinterpret_cast<std::uintptr_t*>(0x007d3ff8u);
    rs_fn = *reinterpret_cast<RwSetState_t*>(vtable_obj + 0x20u);
    rs_fn(6, 1);
    rs_fn(8, 1);
}
// --- 0x00403050 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// LANE 3: page-level write tracking (Core/ShadowTrack.h) -- no region spec; every page
// the call writes plus the caller's 4 KB stack window is compared, pre-state restored
// between the two runs. Verifies EFFECTS; read the pages= detail on a DIVERGENT row.
extern "C" __declspec(dllexport) void __cdecl PreRaceLoadingScreenDraw() {
    SHADOW_AB_COUNTER(ab, "PreRaceLoadingScreenDraw", 0x00403050u, ShadowAB::kPhaseRace);
    ShadowAB::RunTracked(ab, PreRaceLoadingScreenDraw_impl, SHADOW_STACK_WINDOW());
}
RH_ScopedInstall(PreRaceLoadingScreenDraw, 0x00403050);

// ───────────────────────────────────────────────────────────────────────────
// 0x0042bcb0  PlayerNameSpriteDraw
//
// Original: FUN_0042bcb0 (222 bytes, 0x0042bcb0..0x0042bd8d)
// Signature:
//   void FUN_0042bcb0(int param_1, float param_2, float param_3,
//                     undefined4 param_4, int param_5)
//
// Body:
//   Stack cookie guard (DAT_00616038 ^ retaddr).
//   Coordinate transform — 4 floats:
//     local_24 = param_2 * _DAT_005cd5a8 + _DAT_005cd18c
//     local_20 = _DAT_005cc724 + param_3 * _DAT_005cc560
//     local_1c = param_2 * _DAT_005cd5a8 - _DAT_005cd18c
//     local_18 = param_3 * _DAT_005cc560 - _DAT_005cc724
//   Zero 16-byte local_14[0..3].
//   mode_byte = (&DAT_007e96fc)[param_1 * 0x80]
//   if (mode_byte == 2):
//     copy bytes from "keyboard" string at 0x005cd778 (9 bytes) into local_14
//   else if (mode_byte == 1):
//     local_14[0..6]: dword DAT_005cd770, word DAT_005cd774, byte DAT_005cd776
//   if (param_5 != 0): scan for null in local_14, append word DAT_005cd76c
//   ptr = FUN_004c5c00(DAT_00636ac8, &local_14)
//   if (ptr != 0):
//     uVar4 = *ptr
//     FUN_004b5750(*DAT_007d3ff8, &local_1c, &local_24, &param_4, uVar4)
//
// Constants cited:
//   stride: 0x80 (128)              [DAT_007e96fc index]
//   mode codes: 1, 2                [body branches]
//   coord-transform globals:        0x005cd5a8 (xScale), 0x005cd18c (xOffset)
//                                   0x005cc560 (yScale), 0x005cc724 (yOffset)
//   collection ptr:                 0x00636ac8
//   driver ptr:                     0x007d3ff8
//
// ref: re/analysis/c0_promotion_frontend_a/0x0042bcb0.md
// ───────────────────────────────────────────────────────────────────────────
// 0x0042bcb0
extern "C" __declspec(dllexport) void __cdecl PlayerNameSpriteDraw(int param_1, float param_2, float param_3, std::uint32_t param_4, int param_5);
static void __cdecl PlayerNameSpriteDraw_impl(
    int param_1, float param_2, float param_3,
    std::uint32_t param_4, int param_5) {
    // Coordinate-transform constants (read at body).
    const float xScale  = *reinterpret_cast<const float*>(0x005cd5a8u);
    const float xOffset = *reinterpret_cast<const float*>(0x005cd18cu);
    const float yScale  = *reinterpret_cast<const float*>(0x005cc560u);
    const float yOffset = *reinterpret_cast<const float*>(0x005cc724u);

    float local_24 = param_2 * xScale + xOffset;
    float local_20 = yOffset + param_3 * yScale;
    float local_1c = param_2 * xScale - xOffset;
    float local_18 = param_3 * yScale - yOffset;

    // Suppress unused-warning: pair captured for caller via &local_24 / &local_1c.
    (void)local_20;
    (void)local_18;

    // 16-byte name buffer.
    std::uint8_t local_14[16];
    std::memset(local_14, 0, sizeof(local_14));

    // Mode-byte lookup: byte at DAT_007e96fc + param_1 * 0x80.
    const std::uint8_t mode_byte = *reinterpret_cast<const std::uint8_t*>(
        0x007e96fcu + static_cast<std::uintptr_t>(param_1) * 0x80u);

    if (mode_byte == 2) {
        // Copy 9 bytes from string "keyboard" at 0x005cd778 into local_14.
        const auto* src = reinterpret_cast<const std::uint8_t*>(0x005cd778u);
        for (int i = 0; i < 9; ++i) local_14[i] = src[i];
    } else if (mode_byte == 1) {
        // Build name from 3 globals: dword DAT_005cd770, word DAT_005cd774, byte DAT_005cd776.
        // Layout from original: local_14[0..3] = dword, [4..5] = word, [6] = byte.
        *reinterpret_cast<std::uint32_t*>(&local_14[0]) =
            *reinterpret_cast<const std::uint32_t*>(0x005cd770u);
        *reinterpret_cast<std::uint16_t*>(&local_14[4]) =
            *reinterpret_cast<const std::uint16_t*>(0x005cd774u);
        local_14[6] = *reinterpret_cast<const std::uint8_t*>(0x005cd776u);
    }
    // else: local_14 remains zero.

    if (param_5 != 0) {
        // Scan for null terminator; append word DAT_005cd76c there.
        int j = 0;
        while (j < 14 && local_14[j] != 0) ++j;
        *reinterpret_cast<std::uint16_t*>(&local_14[j]) =
            *reinterpret_cast<const std::uint16_t*>(0x005cd76cu);
    }

    // Lookup sprite by name.
    const std::uint32_t collection = *reinterpret_cast<const std::uint32_t*>(0x00636ac8u);
    std::uint32_t* puVar2 = s_FUN_004c5c00(collection, local_14);
    if (puVar2 != nullptr) {
        std::uint32_t handle = *puVar2;
        const std::uint32_t driver = *reinterpret_cast<const std::uint32_t*>(0x007d3ff8u);
        // Draw call: (driver, &local_1c, &local_24, &param_4, handle).
        s_FUN_004b5750(driver, &local_1c, &local_24, &param_4, handle);
    }
}
// --- 0x0042bcb0 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// LANE 3: page-level write tracking (Core/ShadowTrack.h) -- no region spec; every page
// the call writes plus the caller's 4 KB stack window is compared, pre-state restored
// between the two runs. Verifies EFFECTS; read the pages= detail on a DIVERGENT row.
extern "C" __declspec(dllexport) void __cdecl PlayerNameSpriteDraw(int param_1, float param_2, float param_3, std::uint32_t param_4, int param_5) {
    SHADOW_AB_COUNTER(ab, "PlayerNameSpriteDraw", 0x0042bcb0u, ShadowAB::kPhaseRace);
    ShadowAB::RunTracked(ab, PlayerNameSpriteDraw_impl, SHADOW_STACK_WINDOW(), param_1, param_2, param_3, param_4, param_5);
}
RH_ScopedInstall(PlayerNameSpriteDraw, 0x0042bcb0);

// ───────────────────────────────────────────────────────────────────────────
// FUN_00431f30 (page-id dispatch: FUN_00431d90 mass-clear, then one panel flag per page
// id) was ported here first as FrontendPageFlagSet; its one-body-per-RVA copy is
// FrontendPageIdDispatch in Frontend/MenuLeaves_af6.cpp (identical case set, targets and
// values). The duplicate that sat here was removed 2026-10-16 (dupinstall).
// ref: re/analysis/options_menu/0x00431f30.md
// ───────────────────────────────────────────────────────────────────────────
