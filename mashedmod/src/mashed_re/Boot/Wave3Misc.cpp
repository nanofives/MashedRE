// Mashed RE — Boot/Wave3Misc.cpp
// Wave 3 session 5 — panel-piz callees cluster.
//
// Six C2 reimplementations from the post-Panel.piz boot init sequence
// (all called from FUN_00402750 after Panel.piz is mounted):
//
//   0x00412890  PanelIndexBufferInit — fills short[] quad-to-tri index buffer
//                                      + stores "headlight" texture handle
//   0x0041c100  KCPanelLoad         — loads KCPanel.dff into struct at 0x0063cda4
//   0x0041d8b0  StartlightsLoad     — loads startlights.dff into struct at 0x0063d55c
//   0x0041db90  RacePanelLoad       — loads RacePanel.dff into struct at 0x0063d5a0
//   0x0041eaa0  ShadowRenderInit    — creates render objects + shadow-map RTs
//   0x00420d00  PanelSlotIndexInit  — clears 0x90 bytes, sets 4 slot indices 0..3
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/promote_c2_panel_piz_callees/00412890.md
//   re/analysis/promote_c2_panel_piz_callees/0041c100.md
//   re/analysis/promote_c2_panel_piz_callees/0041d8b0.md
//   re/analysis/promote_c2_panel_piz_callees/0041db90.md
//   re/analysis/promote_c2_panel_piz_callees/0041eaa0.md
//   re/analysis/promote_c2_panel_piz_callees/00420d00.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Callee forward declarations (all passthrough to originals)
// ---------------------------------------------------------------------------

// FUN_0040bb30 — texture name-to-handle lookup: (const char*) → uint32
using FUN_0040bb30_t = std::uint32_t (__cdecl*)(const char*);
static FUN_0040bb30_t const s_FUN_0040bb30 =
    reinterpret_cast<FUN_0040bb30_t>(0x0040bb30u);

// FUN_0042a5d0 — DFF loader: (const char* name, int, int) → int (object ptr)
using FUN_0042a5d0_t = std::int32_t (__cdecl*)(const char*, std::int32_t, std::int32_t);
static FUN_0042a5d0_t const s_FUN_0042a5d0 =
    reinterpret_cast<FUN_0042a5d0_t>(0x0042a5d0u);

// FUN_004b6520 — memset-style struct clear: (void* dst, uint32 size) → void
using FUN_004b6520_t = void (__cdecl*)(void*, std::uint32_t);
static FUN_004b6520_t const s_FUN_004b6520 =
    reinterpret_cast<FUN_004b6520_t>(0x004b6520u);

// FUN_004b51d0 — DFF sub-object binder: (int obj, void* struct_base, int count, int, int)
using FUN_004b51d0_t = void (__cdecl*)(std::int32_t, void*, std::int32_t, std::int32_t, std::int32_t);
static FUN_004b51d0_t const s_FUN_004b51d0 =
    reinterpret_cast<FUN_004b51d0_t>(0x004b51d0u);

// FUN_004b52f0 — per-sub-object flag setter: (int elem, int flag, int val)
using FUN_004b52f0_t = void (__cdecl*)(std::int32_t, std::int32_t, std::int32_t);
static FUN_004b52f0_t const s_FUN_004b52f0 =
    reinterpret_cast<FUN_004b52f0_t>(0x004b52f0u);

// FUN_004c0b30 — render object factory: (void) → uint32 handle
using FUN_004c0b30_t = std::uint32_t (__cdecl*)();
static FUN_004c0b30_t const s_FUN_004c0b30 =
    reinterpret_cast<FUN_004c0b30_t>(0x004c0b30u);

// FUN_004770c0 — render obj texture-slot binder:
//   (uint32 render_obj, int type_flag, int slot_count, uint32 tex_handle)
using FUN_004770c0_t = void (__cdecl*)(std::uint32_t, std::int32_t, std::int32_t, std::uint32_t);
static FUN_004770c0_t const s_FUN_004770c0 =
    reinterpret_cast<FUN_004770c0_t>(0x004770c0u);

// FUN_00476cb0 — render obj property setter:
//   (uint32 render_obj, int prop_index, int prop_val)
using FUN_00476cb0_t = void (__cdecl*)(std::uint32_t, std::int32_t, std::int32_t);
static FUN_00476cb0_t const s_FUN_00476cb0 =
    reinterpret_cast<FUN_00476cb0_t>(0x00476cb0u);

// FUN_004b48e0 — render-target allocator:
//   (int width, int height, void** out_handle, void** out_ptr)
using FUN_004b48e0_t = void (__cdecl*)(std::int32_t, std::int32_t, void**, void**);
static FUN_004b48e0_t const s_FUN_004b48e0 =
    reinterpret_cast<FUN_004b48e0_t>(0x004b48e0u);

// FUN_004c1c80 — render target UV scale setter: (uint32 handle, float* scale_xy)
using FUN_004c1c80_t = void (__cdecl*)(std::uint32_t, const float*);
static FUN_004c1c80_t const s_FUN_004c1c80 =
    reinterpret_cast<FUN_004c1c80_t>(0x004c1c80u);

// FUN_004c1c10 — render target mode setter: (uint32 handle, int mode)
using FUN_004c1c10_t = void (__cdecl*)(std::uint32_t, std::int32_t);
static FUN_004c1c10_t const s_FUN_004c1c10 =
    reinterpret_cast<FUN_004c1c10_t>(0x004c1c10u);

// ---------------------------------------------------------------------------
// Global address constants (all cited from analysis notes)
// ---------------------------------------------------------------------------

// DAT_0063bc54 — receives "headlight" texture handle (0x00412890 body)
static std::uint32_t* const g_DAT_0063bc54 =
    reinterpret_cast<std::uint32_t*>(0x0063bc54u);

// DAT_0063cda4 — 28-byte struct base for KCPanel DFF sub-objects (0x0041c100 body)
static std::uint8_t*  const g_DAT_0063cda4 =
    reinterpret_cast<std::uint8_t*>(0x0063cda4u);
// DAT_0063cdb4 — KCPanel DFF object ptr (0x0041c100 body)
static std::int32_t*  const g_DAT_0063cdb4 =
    reinterpret_cast<std::int32_t*>(0x0063cdb4u);
// DAT_0063cdb8 — KCPanel *(obj+4) (0x0041c100 body)
static std::int32_t*  const g_DAT_0063cdb8 =
    reinterpret_cast<std::int32_t*>(0x0063cdb8u);
// DAT_0063cdbc — KCPanel flag = 1 (0x0041c100 body)
static std::int32_t*  const g_DAT_0063cdbc =
    reinterpret_cast<std::int32_t*>(0x0063cdbcu);
// DAT_0063cda8 — sub-struct base for triple-deref color write (shared)
static std::int32_t*  const g_DAT_0063cda8 =
    reinterpret_cast<std::int32_t*>(0x0063cda8u);

// DAT_0063d55c — 48-byte struct base for startlights sub-objects (0x0041d8b0)
static std::uint8_t*  const g_DAT_0063d55c =
    reinterpret_cast<std::uint8_t*>(0x0063d55cu);
// DAT_0063d57c — startlights DFF object ptr (0x0041d8b0)
static std::int32_t*  const g_DAT_0063d57c =
    reinterpret_cast<std::int32_t*>(0x0063d57cu);
// DAT_0063d580 — startlights *(obj+4) (0x0041d8b0)
static std::int32_t*  const g_DAT_0063d580 =
    reinterpret_cast<std::int32_t*>(0x0063d580u);

// DAT_0063d5a0 — 84-byte struct base for RacePanel sub-objects (0x0041db90)
static std::uint8_t*  const g_DAT_0063d5a0 =
    reinterpret_cast<std::uint8_t*>(0x0063d5a0u);
// DAT_0063d5e0 — RacePanel DFF object ptr (0x0041db90)
static std::int32_t*  const g_DAT_0063d5e0 =
    reinterpret_cast<std::int32_t*>(0x0063d5e0u);
// DAT_0063d5e4 — RacePanel *(obj+4) (0x0041db90)
static std::int32_t*  const g_DAT_0063d5e4 =
    reinterpret_cast<std::int32_t*>(0x0063d5e4u);
// DAT_0063d5e8 — RacePanel flag = 1 (0x0041db90)
static std::int32_t*  const g_DAT_0063d5e8 =
    reinterpret_cast<std::int32_t*>(0x0063d5e8u);

// DAT_0063d840 — render-object slot array base (0x0041eaa0 loop)
static std::uint32_t* const g_DAT_0063d840 =
    reinterpret_cast<std::uint32_t*>(0x0063d840u);
// DAT_0063e49c — pointer to render-obj array at DAT_0063d8a0 (0x0041eaa0)
static std::uint32_t* const g_DAT_0063e49c =
    reinterpret_cast<std::uint32_t*>(0x0063e49cu);
// DAT_0063d8a0 — first render-obj array base (pointed to by DAT_0063e49c)
static std::uint32_t* const g_DAT_0063d8a0 =
    reinterpret_cast<std::uint32_t*>(0x0063d8a0u);
// DAT_005cd268 — address of texture name string (first tex bind) (0x0041eaa0)
static const char*    const g_PTR_005cd268 =
    reinterpret_cast<const char*>(0x005cd268u);
// DAT_0063e4a0 — pointer to second render-obj array at DAT_0063d860 (0x0041eaa0)
static std::uint32_t* const g_DAT_0063e4a0 =
    reinterpret_cast<std::uint32_t*>(0x0063e4a0u);
// DAT_0063d860 — second render-obj array base (pointed to by DAT_0063e4a0)
static std::uint32_t* const g_DAT_0063d860 =
    reinterpret_cast<std::uint32_t*>(0x0063d860u);
// DAT_00636acc — guard flag: skip RT alloc if non-zero (0x0041eaa0)
static std::int32_t*  const g_DAT_00636acc =
    reinterpret_cast<std::int32_t*>(0x00636accu);
// DAT_0063e490 — first render-target handle slot (0x0041eaa0)
static std::uint32_t* const g_DAT_0063e490 =
    reinterpret_cast<std::uint32_t*>(0x0063e490u);
// DAT_0063d854 — ptr paired with first RT alloc (0x0041eaa0)
static void**         const g_DAT_0063d854 =
    reinterpret_cast<void**>(0x0063d854u);
// DAT_0063e494 — second render-target handle slot (0x0041eaa0)
static std::uint32_t* const g_DAT_0063e494 =
    reinterpret_cast<std::uint32_t*>(0x0063e494u);
// DAT_0063d858 — ptr paired with second RT alloc (0x0041eaa0)
static void**         const g_DAT_0063d858 =
    reinterpret_cast<void**>(0x0063d858u);

// DAT_0063e4b8 — 144-byte struct base for 4-slot array (0x00420d00)
static std::uint8_t*  const g_DAT_0063e4b8 =
    reinterpret_cast<std::uint8_t*>(0x0063e4b8u);
// Four globals at stride 0x24 from DAT_0063e4b8 (0x00420d00 writes)
// 0x0063e4d0 = base + 0x18; 0x0063e4f4 = base + 0x3c; etc.
static std::int32_t*  const g_DAT_0063e4d0 =
    reinterpret_cast<std::int32_t*>(0x0063e4d0u);
static std::int32_t*  const g_DAT_0063e4f4 =
    reinterpret_cast<std::int32_t*>(0x0063e4f4u);
static std::int32_t*  const g_DAT_0063e518 =
    reinterpret_cast<std::int32_t*>(0x0063e518u);
static std::int32_t*  const g_DAT_0063e53c =
    reinterpret_cast<std::int32_t*>(0x0063e53cu);

// ---------------------------------------------------------------------------
// PanelIndexBufferInit  --  0x00412890
//
// Original: FUN_00412890 (85 bytes, 0x00412890–0x004128d4).
// Decompiled body (verbatim from analysis note 00412890.md):
//   sVar3 = 0; psVar2 = &DAT_0089d4a2.
//   while ((int)psVar2 < 0x8a94a2) [stride: psVar2 += 6 shorts = 12 bytes]:
//     sVar1 = sVar3 * 4
//     psVar2[-1] = sVar1; *psVar2 = sVar1+1; psVar2[1] = sVar1+3;
//     psVar2[2]  = sVar1; psVar2[3] = sVar1+2; psVar2[4] = sVar1+3
//     sVar3++
//   DAT_0063bc54 = FUN_0040bb30("headlight")
//
// Fills a short[] index buffer at 0x0089d4a0 with quad-to-triangle fan split
// indices (CCW winding pattern {0,1,3,0,2,3} per quad). Then looks up the
// "headlight" texture handle and stores it at 0x0063bc54.
//
// Constants:
//   loop base  0x0089d4a2  — initial psVar2 (writes from psVar2-1 = 0x0089d4a0)
//   loop limit 0x008a94a2  — exclusive upper bound
//   stride     6 shorts (12 bytes) per iteration
//   sVar1 = sVar3 * 4  — 4 vertices per quad
//   index offsets {0,1,3,0,2,3}  — CCW quad-to-triangle split
//   DAT_0063bc54  — "headlight" texture handle output
//
// Caller: FUN_00402750 (post-Panel.piz init sequence).
// ---------------------------------------------------------------------------

// 0x00412890
extern "C" __declspec(dllexport) void __cdecl PanelIndexBufferInit() {
    // loop base: psVar2 = &DAT_0089d4a2 (write starts at psVar2-1 = 0x0089d4a0)
    std::int16_t* psVar2 = reinterpret_cast<std::int16_t*>(0x0089d4a2u);
    std::int16_t  sVar3 = 0;

    // while ((int)psVar2 < 0x8a94a2) stride: +6 shorts (12 bytes)
    while (reinterpret_cast<std::uintptr_t>(psVar2) < 0x008a94a2u) {
        const std::int16_t sVar1 = static_cast<std::int16_t>(sVar3 * 4);
        // CCW quad-to-triangle split: {0,1,3,0,2,3}
        psVar2[-1] = sVar1;
        psVar2[ 0] = static_cast<std::int16_t>(sVar1 + 1);
        psVar2[ 1] = static_cast<std::int16_t>(sVar1 + 3);
        psVar2[ 2] = sVar1;
        psVar2[ 3] = static_cast<std::int16_t>(sVar1 + 2);
        psVar2[ 4] = static_cast<std::int16_t>(sVar1 + 3);
        sVar3 = static_cast<std::int16_t>(sVar3 + 1);
        psVar2 += 6; // stride: 6 shorts = 12 bytes
    }

    // DAT_0063bc54 = FUN_0040bb30("headlight")
    *g_DAT_0063bc54 = s_FUN_0040bb30("headlight");
}

RH_ScopedInstall(PanelIndexBufferInit, 0x00412890);


// ---------------------------------------------------------------------------
// KCPanelLoad  --  0x0041c100
//
// Original: FUN_0041c100 (116 bytes, 0x0041c100–0x0041c173).
// Decompiled body (verbatim from analysis note 0041c100.md):
//   iVar1 = FUN_0042a5d0("KCPanel.dff", 0, 0)
//   FUN_004b6520(&DAT_0063cda4, 0x1c)   — clear 28 bytes
//   DAT_0063cdb8 = *(iVar1 + 4)
//   DAT_0063cdbc = 1
//   DAT_0063cdb4 = iVar1
//   FUN_004b51d0(iVar1, &DAT_0063cda4, 4, 0, 0)   — bind 4 sub-objects
//   — triple-deref color write: *(*(*(DAT_0063cda8 + 0x18) + 0x20) + 4) = 0xff323232
//
// Constants:
//   "KCPanel.dff" — DFF asset name
//   0x1c (28)     — FUN_004b6520 struct clear size
//   4             — FUN_004b51d0 sub-object count
//   0x18 (24)     — first deref offset from DAT_0063cda8
//   0x20 (32)     — second deref offset
//   0x4  (4)      — final write offset
//   0xff323232    — ARGB color (A=0xff R=0x32 G=0x32 B=0x32)
// ---------------------------------------------------------------------------

// 0x0041c100
extern "C" __declspec(dllexport) void __cdecl KCPanelLoad() {
    // iVar1 = FUN_0042a5d0("KCPanel.dff", 0, 0)
    const std::int32_t iVar1 = s_FUN_0042a5d0("KCPanel.dff", 0, 0);

    // FUN_004b6520(&DAT_0063cda4, 0x1c) — clears 28 bytes at struct base
    s_FUN_004b6520(g_DAT_0063cda4, 0x1cu);

    // DAT_0063cdb8 = *(iVar1 + 4)
    *g_DAT_0063cdb8 = *reinterpret_cast<const std::int32_t*>(iVar1 + 4);

    // DAT_0063cdbc = 1
    *g_DAT_0063cdbc = 1;

    // DAT_0063cdb4 = iVar1
    *g_DAT_0063cdb4 = iVar1;

    // FUN_004b51d0(iVar1, &DAT_0063cda4, 4, 0, 0) — bind 4 sub-objects
    s_FUN_004b51d0(iVar1, g_DAT_0063cda4, 4, 0, 0);

    // Triple-deref color write: *(*(*(DAT_0063cda8 + 0x18) + 0x20) + 4) = 0xff323232
    // 0x18 = offset into struct at DAT_0063cda8; 0x20 = second level; 0x4 = final write
    std::int32_t* p1 = reinterpret_cast<std::int32_t*>(
        *reinterpret_cast<const std::int32_t*>(
            reinterpret_cast<const std::uint8_t*>(g_DAT_0063cda8) + 0x18));
    std::int32_t* p2 = reinterpret_cast<std::int32_t*>(
        *reinterpret_cast<const std::int32_t*>(
            reinterpret_cast<const std::uint8_t*>(p1) + 0x20));
    *reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uint8_t*>(p2) + 0x4) = 0xff323232u;
}

RH_ScopedInstall(KCPanelLoad, 0x0041c100);


// ---------------------------------------------------------------------------
// StartlightsLoad  --  0x0041d8b0
//
// Original: FUN_0041d8b0 (95 bytes, 0x0041d8b0–0x0041d90e).
// Decompiled body (verbatim from analysis note 0041d8b0.md):
//   iVar1 = FUN_0042a5d0("startlights.dff", 0, 0)
//   FUN_004b6520(&DAT_0063d55c, 0x30)   — clear 48 bytes
//   DAT_0063d580 = *(iVar1 + 4)
//   DAT_0063d57c = iVar1
//   FUN_004b51d0(iVar1, &DAT_0063d55c, 8, 0, 0)   — bind 8 sub-objects
//   loop puVar2 in [0x0063d55c .. 0x0063d574), stride 4:
//     FUN_004b52f0(*puVar2, 0x40, 1)
//
// Constants:
//   "startlights.dff"  — DFF asset name
//   0x30 (48)          — struct clear size
//   8                  — sub-object count for FUN_004b51d0
//   loop base 0x0063d55c, limit 0x0063d574  — 6 iters × 4 bytes
//   0x40 (64)          — FUN_004b52f0 flag per element
// ---------------------------------------------------------------------------

// 0x0041d8b0
extern "C" __declspec(dllexport) void __cdecl StartlightsLoad() {
    // iVar1 = FUN_0042a5d0("startlights.dff", 0, 0)
    const std::int32_t iVar1 = s_FUN_0042a5d0("startlights.dff", 0, 0);

    // FUN_004b6520(&DAT_0063d55c, 0x30) — clears 48 bytes
    s_FUN_004b6520(g_DAT_0063d55c, 0x30u);

    // DAT_0063d580 = *(iVar1 + 4)
    *g_DAT_0063d580 = *reinterpret_cast<const std::int32_t*>(iVar1 + 4);

    // DAT_0063d57c = iVar1
    *g_DAT_0063d57c = iVar1;

    // FUN_004b51d0(iVar1, &DAT_0063d55c, 8, 0, 0) — bind 8 sub-objects
    s_FUN_004b51d0(iVar1, g_DAT_0063d55c, 8, 0, 0);

    // loop: puVar2 from &DAT_0063d55c while puVar2 < 0x0063d574 (6 iters, stride 4)
    std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(0x0063d55cu);
    while (reinterpret_cast<std::uintptr_t>(puVar2) < 0x0063d574u) {
        // FUN_004b52f0(*puVar2, 0x40, 1) — per-element flag setter
        s_FUN_004b52f0(static_cast<std::int32_t>(*puVar2), 0x40, 1);
        ++puVar2;
    }
}

RH_ScopedInstall(StartlightsLoad, 0x0041d8b0);


// ---------------------------------------------------------------------------
// RacePanelLoad  --  0x0041db90
//
// Original: FUN_0041db90 (147 bytes, 0x0041db90–0x0041dc22).
// Decompiled body (verbatim from analysis note 0041db90.md):
//   iVar2 = FUN_0042a5d0("RacePanel.dff", 0, 0)
//   FUN_004b6520(&DAT_0063d5a0, 0x54)   — clear 84 bytes
//   DAT_0063d5e4 = *(iVar2 + 4)
//   DAT_0063d5e8 = 1
//   DAT_0063d5e0 = iVar2
//   FUN_004b51d0(iVar2, &DAT_0063d5a0, 0x10, 0, 0)   — bind 16 sub-objects
//   loop iVar2=0..15:
//     if iVar2 == 2:
//       iVar1 = (&DAT_0063d5a0)[iVar2]
//       FUN_004b52f0(iVar1, 0x40, 1)
//       *(*(*(iVar1 + 0x18) + 0x20) + 4) = 0xff323232
//
// Constants:
//   "RacePanel.dff"  — DFF asset name
//   0x54 (84)        — struct clear size
//   0x10 (16)        — sub-object count
//   0x2              — only index 2 takes flag+color path
//   0x40 (64)        — FUN_004b52f0 flag
//   0xff323232       — ARGB color A=0xff R=0x32 G=0x32 B=0x32
//   offsets 0x18 / 0x20 / 0x4  — triple-deref chain
// ---------------------------------------------------------------------------

// 0x0041db90
extern "C" __declspec(dllexport) void __cdecl RacePanelLoad() {
    // iVar2 = FUN_0042a5d0("RacePanel.dff", 0, 0)
    const std::int32_t iVar2 = s_FUN_0042a5d0("RacePanel.dff", 0, 0);

    // FUN_004b6520(&DAT_0063d5a0, 0x54) — clears 84 bytes
    s_FUN_004b6520(g_DAT_0063d5a0, 0x54u);

    // DAT_0063d5e4 = *(iVar2 + 4)
    *g_DAT_0063d5e4 = *reinterpret_cast<const std::int32_t*>(iVar2 + 4);

    // DAT_0063d5e8 = 1
    *g_DAT_0063d5e8 = 1;

    // DAT_0063d5e0 = iVar2
    *g_DAT_0063d5e0 = iVar2;

    // FUN_004b51d0(iVar2, &DAT_0063d5a0, 0x10, 0, 0) — bind 16 sub-objects
    s_FUN_004b51d0(iVar2, g_DAT_0063d5a0, 0x10, 0, 0);

    // loop iVar2=0..15: only index 2 gets flag+color
    const std::int32_t* slots =
        reinterpret_cast<const std::int32_t*>(g_DAT_0063d5a0);
    for (int i = 0; i < 0x10; ++i) {
        if (i == 2) {
            const std::int32_t iVar1 = slots[i];
            // FUN_004b52f0(iVar1, 0x40, 1)
            s_FUN_004b52f0(iVar1, 0x40, 1);
            // Triple-deref color write: *(*(*(iVar1 + 0x18) + 0x20) + 4) = 0xff323232
            std::int32_t* p1 = reinterpret_cast<std::int32_t*>(
                *reinterpret_cast<const std::int32_t*>(
                    reinterpret_cast<const std::uint8_t*>(iVar1) + 0x18));
            std::int32_t* p2 = reinterpret_cast<std::int32_t*>(
                *reinterpret_cast<const std::int32_t*>(
                    reinterpret_cast<const std::uint8_t*>(p1) + 0x20));
            *reinterpret_cast<std::uint32_t*>(
                reinterpret_cast<std::uint8_t*>(p2) + 0x4) = 0xff323232u;
        }
    }
}

RH_ScopedInstall(RacePanelLoad, 0x0041db90);


// ---------------------------------------------------------------------------
// ShadowRenderInit  --  0x0041eaa0
//
// Original: FUN_0041eaa0 (270 bytes, 0x0041eaa0–0x0041ebad).
// Decompiled body (verbatim from analysis note 0041eaa0.md):
//
//   loop puVar2 in [0x0063d840 .. 0x0063d850), stride 4 (4 iters):
//     *puVar2 = FUN_004c0b30()
//   DAT_0063e49c = &DAT_0063d8a0
//   uVar1 = FUN_0040bb30(&DAT_005cd268)
//   FUN_004770c0(DAT_0063e49c, 0x80c, 4, uVar1)
//   FUN_00476cb0(DAT_0063e49c, 5, 2)
//   DAT_0063e4a0 = &DAT_0063d860
//   uVar1 = FUN_0040bb30("car_shadow")
//   FUN_004770c0(DAT_0063e4a0, 0x80c, 4, uVar1)
//   if DAT_00636acc == 0:
//     local_8 = 0x3f400000   (0.75f)
//     local_4 = 0x3f400000   (0.75f)
//     FUN_004b48e0(0x100, 0x100, &DAT_0063e490, &DAT_0063d854)
//     FUN_004c1c80(DAT_0063e490, &local_8)
//     FUN_004c1c10(DAT_0063e490, 2)
//     FUN_004b48e0(0x40, 0x40, &DAT_0063e494, &DAT_0063d858)
//     FUN_004c1c80(DAT_0063e494, &local_8)
//     FUN_004c1c10(DAT_0063e494, 2)
//
// Constants:
//   loop base 0x0063d840 / limit 0x0063d850   — 4 render-obj slots
//   DAT_0063e49c / DAT_0063d8a0               — first obj array ptr store
//   0x80c (2060) / 4                          — texture-slot bind args
//   DAT_005cd268                              — first texture name ptr
//   "car_shadow"                              — second texture name
//   5 / 2                                     — property index/value
//   DAT_0063e4a0 / DAT_0063d860               — second obj array ptr store
//   0x3f400000 = 0.75f                        — UV scale
//   0x100 / 0x100                             — 256×256 first RT
//   0x40  / 0x40                              — 64×64  second RT
//   mode 2                                    — FUN_004c1c10 mode
// ---------------------------------------------------------------------------

// 0x0041eaa0
extern "C" __declspec(dllexport) void __cdecl ShadowRenderInit() {
    // loop: fill 4 render-obj slots at 0x0063d840..0x0063d84f
    {
        std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(0x0063d840u);
        while (reinterpret_cast<std::uintptr_t>(puVar2) < 0x0063d850u) {
            *puVar2 = s_FUN_004c0b30();
            ++puVar2;
        }
    }

    // DAT_0063e49c = &DAT_0063d8a0
    *g_DAT_0063e49c = reinterpret_cast<std::uintptr_t>(g_DAT_0063d8a0);

    // uVar1 = FUN_0040bb30(&DAT_005cd268) — texture name lookup via ptr
    const std::uint32_t uVar1a = s_FUN_0040bb30(g_PTR_005cd268);

    // FUN_004770c0(DAT_0063e49c, 0x80c, 4, uVar1)
    s_FUN_004770c0(*g_DAT_0063e49c, 0x80c, 4, uVar1a);

    // FUN_00476cb0(DAT_0063e49c, 5, 2)
    s_FUN_00476cb0(*g_DAT_0063e49c, 5, 2);

    // DAT_0063e4a0 = &DAT_0063d860
    *g_DAT_0063e4a0 = reinterpret_cast<std::uintptr_t>(g_DAT_0063d860);

    // uVar1 = FUN_0040bb30("car_shadow")
    const std::uint32_t uVar1b = s_FUN_0040bb30("car_shadow");

    // FUN_004770c0(DAT_0063e4a0, 0x80c, 4, uVar1)
    s_FUN_004770c0(*g_DAT_0063e4a0, 0x80c, 4, uVar1b);

    // if DAT_00636acc == 0: shadow-map RT alloc
    if (*g_DAT_00636acc == 0) {
        // local_8 = 0x3f400000 (0.75f), local_4 = 0x3f400000 (0.75f)
        std::uint32_t uv_scale_bits = 0x3f400000u; // IEEE 754 = 0.75f
        float local_uv[2];
        std::memcpy(&local_uv[0], &uv_scale_bits, 4);
        std::memcpy(&local_uv[1], &uv_scale_bits, 4);

        // FUN_004b48e0(0x100, 0x100, &DAT_0063e490, &DAT_0063d854)
        s_FUN_004b48e0(0x100, 0x100,
                       reinterpret_cast<void**>(g_DAT_0063e490),
                       reinterpret_cast<void**>(g_DAT_0063d854));
        // FUN_004c1c80(DAT_0063e490, &local_uv)
        s_FUN_004c1c80(*g_DAT_0063e490, local_uv);
        // FUN_004c1c10(DAT_0063e490, 2)
        s_FUN_004c1c10(*g_DAT_0063e490, 2);

        // FUN_004b48e0(0x40, 0x40, &DAT_0063e494, &DAT_0063d858)
        s_FUN_004b48e0(0x40, 0x40,
                       reinterpret_cast<void**>(g_DAT_0063e494),
                       reinterpret_cast<void**>(g_DAT_0063d858));
        // FUN_004c1c80(DAT_0063e494, &local_uv)
        s_FUN_004c1c80(*g_DAT_0063e494, local_uv);
        // FUN_004c1c10(DAT_0063e494, 2)
        s_FUN_004c1c10(*g_DAT_0063e494, 2);
    }
}

RH_ScopedInstall(ShadowRenderInit, 0x0041eaa0);


// ---------------------------------------------------------------------------
// PanelSlotIndexInit  --  0x00420d00
//
// Original: FUN_00420d00 (58 bytes, 0x00420d00–0x00420d39).
// Decompiled body (verbatim from analysis note 00420d00.md):
//   FUN_004b6520(&DAT_0063e4b8, 0x90)   — clears 144 bytes
//   DAT_0063e4d0 = 0
//   DAT_0063e4f4 = 1
//   DAT_0063e518 = 2
//   DAT_0063e53c = 3
//
// Clears a 4-element struct array (element stride 0x24 bytes) then sets
// the first dword of each element to its sequential index 0..3.
//
// Constants:
//   0x90 (144)   — FUN_004b6520 clear size (= 4 × 0x24)
//   0x0063e4d0   — element 0 index field (= base 0x0063e4b8 + 0x18)
//   0x0063e4f4   — element 1 index field (stride 0x24 = 36 bytes)
//   0x0063e518   — element 2 index field
//   0x0063e53c   — element 3 index field
//   Values 0, 1, 2, 3  — sequential slot indices
// ---------------------------------------------------------------------------

// 0x00420d00
extern "C" __declspec(dllexport) void __cdecl PanelSlotIndexInit() {
    // FUN_004b6520(&DAT_0063e4b8, 0x90) — clears 144 bytes at struct base
    s_FUN_004b6520(g_DAT_0063e4b8, 0x90u);

    // Store sequential indices into the four element index fields
    *g_DAT_0063e4d0 = 0; // slot 0
    *g_DAT_0063e4f4 = 1; // slot 1
    *g_DAT_0063e518 = 2; // slot 2
    *g_DAT_0063e53c = 3; // slot 3
}

RH_ScopedInstall(PanelSlotIndexInit, 0x00420d00);
