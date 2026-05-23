// Mashed RE — Boot/SubsystemInit.cpp
// Boot subsystem init cluster: SubsystemInit, DisplayInit, ViewportInit,
// and SetDefaultViewWindow.
//
// Covers:
//   0x00492270  SubsystemInit          — 3-callee gate: RW_INIT_FN(0) + DisplayInit + ViewportInit
//   0x004921d0  DisplayInit            — display init: RW video-mode + LoadIcon TXD
//   0x00428590  ViewportInit           — viewport bg-color + DAT_0067d974 = π/2 + RW cam ops
//   0x00492e60  SetDefaultViewWindow   — RwCameraSetViewWindow(0.8, 0.8)
//
// Deferred:
//   0x004924f0  DataZeroFill           — DEFERRED (see note at bottom of file)
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/boot_app_init/00492270.md
//   re/analysis/boot_subsystem_d3/0x004921d0.md
//   re/analysis/boot_subsystem_d3/0x00428590.md
//   re/analysis/skeleton_prep_boot_winmain_a/00492e60.md
//   re/analysis/boot_app_init/004924f0.md  (deferred)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations of callees not yet reimplemented in this TU.
// Each callee RVA is pinned as a function pointer cast.
// ---------------------------------------------------------------------------

// FUN_00493710 — RW init gate: calls RwEngineInit/Open/Start chain; returns 1 on success.
// Signature: undefined4 FUN_00493710(undefined4 param_1)
// Callee of SubsystemInit (0x00492270).
using FUN_00493710_t = std::int32_t (__cdecl*)(std::int32_t);
static FUN_00493710_t const s_FUN_00493710 =
    reinterpret_cast<FUN_00493710_t>(0x00493710u);

// FUN_004921d0 — DisplayInit (defined below in this TU; forward ref for use in 0x00492270).
// Declared extern "C" because it is exported from this TU.
extern "C" __declspec(dllexport) void __cdecl DisplayInit();

// FUN_00428590 — ViewportInit (defined below in this TU; forward ref).
extern "C" __declspec(dllexport) std::int32_t __cdecl ViewportInit();

// FUN_004c2f00 — RwEngineGetCurrentMode (C3; implemented in Boot/VideoConfig.cpp).
// Returns current RW video-mode index as short.
using FUN_004c2f00_t = std::int16_t (__cdecl*)();
static FUN_004c2f00_t const s_FUN_004c2f00 =
    reinterpret_cast<FUN_004c2f00_t>(0x004c2f00u);

// FUN_004c2ed0 — RwEngineGetModeInfo (C2; implemented in Render/RwPluginHelpers_o3.cpp).
// Fills a 24-byte mode-info struct.
// Signature: void FUN_004c2ed0(undefined4* buf, int mode_index)
using FUN_004c2ed0_t = void (__cdecl*)(std::uint8_t*, std::int32_t);
static FUN_004c2ed0_t const s_FUN_004c2ed0 =
    reinterpret_cast<FUN_004c2ed0_t>(0x004c2ed0u);

// FUN_00498bc0 — VideoGetRenderWidth (C3; implemented in Boot/VideoConfig.cpp).
using FUN_00498bc0_t = std::uint32_t (__cdecl*)();
static FUN_00498bc0_t const s_FUN_00498bc0 =
    reinterpret_cast<FUN_00498bc0_t>(0x00498bc0u);

// FUN_00498bd0 — VideoGetRenderHeight (C3; implemented in Boot/VideoConfig.cpp).
using FUN_00498bd0_t = std::uint32_t (__cdecl*)();
static FUN_00498bd0_t const s_FUN_00498bd0 =
    reinterpret_cast<FUN_00498bd0_t>(0x00498bd0u);

// FUN_00467110 — viewport-dim setter: void(uint32 width, uint32 height).
// Stub S-3902 in STUBS.md — passthrough to original.
using FUN_00467110_t = void (__cdecl*)(std::uint32_t, std::uint32_t);
static FUN_00467110_t const s_FUN_00467110 =
    reinterpret_cast<FUN_00467110_t>(0x00467110u);

// FUN_0042b890 — screen-width store setter: void(uint32).
// Stub S-3903 in STUBS.md.
using FUN_0042b890_t = void (__cdecl*)(std::uint32_t);
static FUN_0042b890_t const s_FUN_0042b890 =
    reinterpret_cast<FUN_0042b890_t>(0x0042b890u);

// FUN_0042b8a0 — screen-height store setter: void(uint32).
// Stub S-3904 in STUBS.md.
using FUN_0042b8a0_t = void (__cdecl*)(std::uint32_t);
static FUN_0042b8a0_t const s_FUN_0042b8a0 =
    reinterpret_cast<FUN_0042b8a0_t>(0x0042b8a0u);

// FUN_0042d560 — post-dim init step: void(void) (C1).
using FUN_0042d560_t = void (__cdecl*)();
static FUN_0042d560_t const s_FUN_0042d560 =
    reinterpret_cast<FUN_0042d560_t>(0x0042d560u);

// FUN_0042f660 — DefaultViewportCameraInit: void(void) (C1).
using FUN_0042f660_t = void (__cdecl*)();
static FUN_0042f660_t const s_FUN_0042f660 =
    reinterpret_cast<FUN_0042f660_t>(0x0042f660u);

// FUN_004997b0 — Win32ResourceLoader (C3): resource id to ptr+len.
// Signature: void FUN_004997b0(uint16 name_id, const char* type, void** out_ptr, uint32* out_len)
using FUN_004997b0_t = void (__cdecl*)(std::uint32_t, const char*, void**, std::uint32_t*);
static FUN_004997b0_t const s_FUN_004997b0 =
    reinterpret_cast<FUN_004997b0_t>(0x004997b0u);

// FUN_004b3eb0 — RW TXD loader: (ptr, len, 0, 0) -> TXD*.
// Stub S-3905 in STUBS.md.
using FUN_004b3eb0_t = void* (__cdecl*)(void*, std::uint32_t, std::int32_t, std::int32_t);
static FUN_004b3eb0_t const s_FUN_004b3eb0 =
    reinterpret_cast<FUN_004b3eb0_t>(0x004b3eb0u);

// FUN_004c5c00 — TXD lookup-by-name (C2): returns texture ptr.
// Signature: void* FUN_004c5c00(void* txd, const char* name)
using FUN_004c5c00_t = void* (__cdecl*)(void*, const char*);
static FUN_004c5c00_t const s_FUN_004c5c00 =
    reinterpret_cast<FUN_004c5c00_t>(0x004c5c00u);

// FUN_004671a0 — RW camera/op dispatcher (C2): multi-arg, returns handle.
// Observed calls:
//   FUN_004671a0(0, &local_4, 3)   — (slot=0, color-ptr, op=3)
//   FUN_004671a0(0)                — (slot=0) — returns handle
//   FUN_004671a0(0, 0, 1)          — (slot=0, 0, op=1)
//   FUN_004671a0(0xffffffff, ptr)  — (sentinel, RwV2d-ptr) in SetDefaultViewWindow
// Using variadic-style: cdecl with maximal arg shape; callers only consume what they pass.
using FUN_004671a0_3arg_t = std::uint32_t (__cdecl*)(std::uint32_t, void*, std::uint32_t);
using FUN_004671a0_1arg_t = std::uint32_t (__cdecl*)(std::uint32_t);
using FUN_004671a0_2arg_t = std::uint32_t (__cdecl*)(std::uint32_t, void*);
static FUN_004671a0_3arg_t const s_FUN_004671a0_3 =
    reinterpret_cast<FUN_004671a0_3arg_t>(0x004671a0u);
static FUN_004671a0_1arg_t const s_FUN_004671a0_1 =
    reinterpret_cast<FUN_004671a0_1arg_t>(0x004671a0u);
static FUN_004671a0_2arg_t const s_FUN_004671a0_2 =
    reinterpret_cast<FUN_004671a0_2arg_t>(0x004671a0u);

// FUN_004c1bb0 — RW cam op A: void(uint32 handle).
using FUN_004c1bb0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c1bb0_t const s_FUN_004c1bb0 =
    reinterpret_cast<FUN_004c1bb0_t>(0x004c1bb0u);

// FUN_004c1a00 — RW cam op B / test predicate: int32(uint32 handle).
using FUN_004c1a00_t = std::int32_t (__cdecl*)(std::uint32_t);
static FUN_004c1a00_t const s_FUN_004c1a00 =
    reinterpret_cast<FUN_004c1a00_t>(0x004c1a00u);

// FUN_00428450 — camera parameter setter (C2): void(int32, int32).
using FUN_00428450_t = void (__cdecl*)(std::int32_t, std::int32_t);
static FUN_00428450_t const s_FUN_00428450 =
    reinterpret_cast<FUN_00428450_t>(0x00428450u);

// FUN_004c19f0 — RW cam op C: void(uint32 handle).
using FUN_004c19f0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c19f0_t const s_FUN_004c19f0 =
    reinterpret_cast<FUN_004c19f0_t>(0x004c19f0u);

// FUN_004c1be0 — RW cam op D: void(uint32 handle).
using FUN_004c1be0_t = void (__cdecl*)(std::uint32_t);
static FUN_004c1be0_t const s_FUN_004c1be0 =
    reinterpret_cast<FUN_004c1be0_t>(0x004c1be0u);

// FUN_004c1c80 — RwCameraSetViewWindow candidate (C2): void(uint32 handle).
using FUN_004c1c80_t = void (__cdecl*)(std::uint32_t);
static FUN_004c1c80_t const s_FUN_004c1c80 =
    reinterpret_cast<FUN_004c1c80_t>(0x004c1c80u);

// ---------------------------------------------------------------------------
// Globals written by DisplayInit — from analysis note 0x004921d0.md.
// ---------------------------------------------------------------------------
static void** const g_DAT_0077195c =
    reinterpret_cast<void**>(0x0077195cu);       // 0x00492255: global TXD ptr
static void** const g_DAT_00771960 =
    reinterpret_cast<void**>(0x00771960u);       // 0x00492269: global LoadIcon-texture ptr

// ---------------------------------------------------------------------------
// Global written by ViewportInit — from analysis note 0x00428590.md.
// ---------------------------------------------------------------------------
static std::uint32_t* const g_DAT_0067d974 =
    reinterpret_cast<std::uint32_t*>(0x0067d974u); // 0x004285c5: receives π/2 float bits

// ---------------------------------------------------------------------------
// SubsystemInit  --  0x00492270
//
// Original: sub_00492270 (30 bytes, 0x00492270–0x0049228e).
// Decompiled body (verbatim from analysis note 00492270.md):
//   iVar1 = FUN_00493710(0);
//   if (iVar1 == 0) return 0;
//   FUN_004921d0();
//   FUN_00428590();
//   return 1;
//
// Gate: calls RW_INIT_FN(0); fails fast if renderer can't init.
// Then calls DisplayInit + ViewportInit unconditionally. Returns 1 on success.
//
// Constants cited (from analysis table):
//   0x00492270  literal arg 0 to FUN_00493710
//   0x00492278  return 0 on gate-failure
//   0x0049228c  return 1 on success
//
// Caller: sub_00492370 (C2). Callee anti-island: FUN_00493710 C2, DisplayInit C2->C3
//   (this batch), ViewportInit C2->C3 (this batch).
// ---------------------------------------------------------------------------

// 0x00492270
extern "C" __declspec(dllexport) std::int32_t __cdecl SubsystemInit() {
    // 0x00492270: iVar1 = FUN_00493710(0) — RW init gate; literal 0 at 0x00492270.
    const std::int32_t iVar1 = s_FUN_00493710(0);
    // 0x00492278: if iVar1 == 0 → return 0 (gate-failure path).
    if (iVar1 == 0) {
        return 0;
    }
    // Else: call FUN_004921d0 (DisplayInit), then FUN_00428590 (ViewportInit).
    DisplayInit();
    ViewportInit();
    // 0x0049228c: return 1.
    return 1;
}

RH_ScopedInstall(SubsystemInit, 0x00492270);


// ---------------------------------------------------------------------------
// DisplayInit  --  0x004921d0
//
// Original: FUN_004921d0 (155 bytes, 0x004921d0–0x0049226b).
// Decompiled body (verbatim from analysis note 0x004921d0.md):
//   sVar1 = FUN_004c2f00()
//   FUN_004c2ed0(local_18, (int)sVar1)
//   uVar2 = FUN_00498bc0()
//   uVar3 = FUN_00498bd0()
//   FUN_00467110(uVar2, uVar3)
//   FUN_0042b890(uVar2)
//   FUN_0042b8a0(uVar3)
//   FUN_0042d560()
//   FUN_0042f660()
//   local_20 = 0; local_1c = 0;
//   FUN_004997b0(0x194, "RWTEXDICTIONARY", &local_1c, &local_20)
//   DAT_0077195c = FUN_004b3eb0(local_1c, local_20, 0, 0)
//   if DAT_0077195c != 0:
//     DAT_00771960 = FUN_004c5c00(DAT_0077195c, "LoadIcon")
//
// Stubs: S-3902 (FUN_00467110), S-3903 (FUN_0042b890), S-3904 (FUN_0042b8a0),
//        S-3905 (FUN_004b3eb0) — all passthrough to originals.
//
// Constants cited (from analysis table):
//   0x00492227  0x194 (404 dec)      — resource id passed to FUN_004997b0
//   0x00492229  "RWTEXDICTIONARY"    — resource-type string
//   0x0049225f  "LoadIcon"           — texture-name within loaded TXD
//   0x00492255  DAT_0077195c         — global TXD ptr (out)
//   0x00492269  DAT_00771960         — global LoadIcon-texture ptr (out)
//
// Callers: SubsystemInit (0x00492270). Callees: FUN_004c2f00 C3, FUN_004c2ed0 C2,
//   FUN_00498bc0 C2, FUN_00498bd0 C3, FUN_004997b0 C3, FUN_004c5c00 C2.
// ---------------------------------------------------------------------------

// 0x004921d0
extern "C" __declspec(dllexport) void __cdecl DisplayInit() {
    // Stack locals matching Ghidra's observed layout: local_18[24], local_1c, local_20.
    std::uint8_t local_18[24];
    void*        local_1c = nullptr;
    std::uint32_t local_20 = 0;

    // 0x004921d0: sVar1 = FUN_004c2f00() — get current RW video mode index (short).
    const std::int16_t sVar1 = s_FUN_004c2f00();

    // FUN_004c2ed0(local_18, (int)sVar1) — fills local_18[24] with mode info.
    s_FUN_004c2ed0(local_18, static_cast<std::int32_t>(sVar1));

    // uVar2 = FUN_00498bc0() — screen width getter.
    const std::uint32_t uVar2 = s_FUN_00498bc0();

    // uVar3 = FUN_00498bd0() — screen height getter.
    const std::uint32_t uVar3 = s_FUN_00498bd0();

    // FUN_00467110(uVar2, uVar3) — viewport-dim setter (Stub S-3902; passthrough).
    s_FUN_00467110(uVar2, uVar3);

    // FUN_0042b890(uVar2) — width store (Stub S-3903; passthrough).
    s_FUN_0042b890(uVar2);

    // FUN_0042b8a0(uVar3) — height store (Stub S-3904; passthrough).
    s_FUN_0042b8a0(uVar3);

    // FUN_0042d560() — post-dim init step (C1, passthrough).
    s_FUN_0042d560();

    // FUN_0042f660() — DefaultViewportCameraInit (C1, passthrough).
    s_FUN_0042f660();

    // 0x00492227: local_20=0, local_1c=0 then FUN_004997b0(0x194, "RWTEXDICTIONARY", &local_1c, &local_20)
    local_20 = 0;
    local_1c = nullptr;
    s_FUN_004997b0(0x194u, "RWTEXDICTIONARY",
                   reinterpret_cast<void**>(&local_1c),
                   &local_20);

    // 0x00492255: DAT_0077195c = FUN_004b3eb0(local_1c, local_20, 0, 0) (Stub S-3905).
    *g_DAT_0077195c = s_FUN_004b3eb0(local_1c, local_20, 0, 0);

    // 0x00492269: if DAT_0077195c != 0: DAT_00771960 = FUN_004c5c00(DAT_0077195c, "LoadIcon")
    if (*g_DAT_0077195c != nullptr) {
        *g_DAT_00771960 = s_FUN_004c5c00(*g_DAT_0077195c, "LoadIcon");
    }
}

RH_ScopedInstall(DisplayInit, 0x004921d0);


// ---------------------------------------------------------------------------
// ViewportInit  --  0x00428590
//
// Original: FUN_00428590 (128 bytes, 0x00428590–0x0042860f).
// Decompiled body (verbatim from analysis note 0x00428590.md):
//   local_4 = 0xff000000
//   uVar1 = FUN_004671a0(0, &local_4, 3)
//   FUN_004c1bb0(uVar1)
//   uVar1 = FUN_004671a0(0)
//   iVar2 = FUN_004c1a00(uVar1)
//   if iVar2 != 0:
//     _DAT_0067d974 = 0x3fc90fdb  (IEEE 754 = π/2 = 1.5707963f)
//     FUN_00428450(0x20, 0xffffffe0)
//     uVar1 = FUN_004671a0(0); FUN_004c19f0(uVar1)
//   uVar1 = FUN_004671a0(0, 0, 1); FUN_004c1be0(uVar1)
//   return 1
//
// Constants cited (from analysis table):
//   0x00428590  0xff000000  — local_4 (ARGB black: alpha=0xff, RGB=0x000000)
//   0x004285c5  0x3fc90fdb  — IEEE 754 float = 1.5707963 (π/2)
//   0x004285d2  0x20        — first arg to FUN_00428450 (32 dec)
//   0x004285d2  0xffffffe0  — second arg to FUN_00428450 (-32 signed)
//   0x0067d974  DAT_        — global float written with π/2 bits
//
// Callers: SubsystemInit (0x00492270). Callees: FUN_004671a0 C2, FUN_00428450 C2.
// ---------------------------------------------------------------------------

// 0x00428590
extern "C" __declspec(dllexport) std::int32_t __cdecl ViewportInit() {
    // 0x00428590: local_4 = 0xff000000 (ARGB: alpha=0xff, RGB=0x000000 = black).
    std::uint32_t local_4 = 0xff000000u;

    // uVar1 = FUN_004671a0(0, &local_4, 3)
    std::uint32_t uVar1 = s_FUN_004671a0_3(0u,
                                            reinterpret_cast<void*>(&local_4),
                                            3u);

    // FUN_004c1bb0(uVar1) — RW cam op A.
    s_FUN_004c1bb0(uVar1);

    // uVar1 = FUN_004671a0(0)
    uVar1 = s_FUN_004671a0_1(0u);

    // iVar2 = FUN_004c1a00(uVar1) — RW cam op B (test/predicate).
    const std::int32_t iVar2 = s_FUN_004c1a00(uVar1);

    if (iVar2 != 0) {
        // 0x004285c5: _DAT_0067d974 = 0x3fc90fdb (IEEE 754 float = π/2 = 1.5707963).
        *g_DAT_0067d974 = 0x3fc90fdbu;

        // 0x004285d2: FUN_00428450(0x20, 0xffffffe0) → (32, -32).
        s_FUN_00428450(0x20, static_cast<std::int32_t>(0xffffffe0u));

        // uVar1 = FUN_004671a0(0); FUN_004c19f0(uVar1) — RW cam op C.
        uVar1 = s_FUN_004671a0_1(0u);
        s_FUN_004c19f0(uVar1);
    }

    // uVar1 = FUN_004671a0(0, 0, 1); FUN_004c1be0(uVar1) — RW cam op D.
    uVar1 = s_FUN_004671a0_3(0u, nullptr, 1u);
    s_FUN_004c1be0(uVar1);

    // Return 1.
    return 1;
}

RH_ScopedInstall(ViewportInit, 0x00428590);


// ---------------------------------------------------------------------------
// SetDefaultViewWindow  --  0x00492e60
//
// Original: FUN_00492e60 (43 bytes, ~0x00492e60–0x00492e8a).
// Decompiled body (verbatim from analysis note 00492e60.md):
//   local_8 = 0x3f4ccccd   (IEEE 754 single: 0.8f)
//   local_4 = 0x3f4ccccd   (IEEE 754 single: 0.8f)
//   uVar1 = FUN_004671a0(0xffffffff, &local_8)
//   FUN_004c1c80(uVar1)
//   return void
//
// The two stack floats form an RwV2d-shaped struct {width=0.8f, height=0.8f}.
// FUN_004c1c80 is the RwCameraSetViewWindow candidate (C2).
//
// Constants cited (from analysis table):
//   body (~0x00492e65)  0x3f4ccccd  — IEEE 754 = 0.8f (view-window width)
//   body (~0x00492e6a)  0x3f4ccccd  — IEEE 754 = 0.8f (view-window height)
//   body (~0x00492e6f)  0xffffffff  — first arg to FUN_004671a0 (camera handle sentinel)
//                                     [UNCERTAIN U-3922]: Blocks=none; not derivable from decomp.
//
// Callers: 0x00492e90 (FrameDispatch, C2); 0x004924f0 (DataZeroFill, C2).
// Callees: FUN_004671a0 C2, FUN_004c1c80 C2.
// ---------------------------------------------------------------------------

// 0x00492e60
extern "C" __declspec(dllexport) void __cdecl SetDefaultViewWindow() {
    // body (~0x00492e65): local_8 = 0x3f4ccccd (IEEE 754 = 0.8f).
    std::uint32_t local_8 = 0x3f4ccccdu;
    // body (~0x00492e6a): local_4 = 0x3f4ccccd (IEEE 754 = 0.8f).
    std::uint32_t local_4 = 0x3f4ccccdu;

    // body (~0x00492e6f): uVar1 = FUN_004671a0(0xffffffff, &local_8)
    // First arg is 0xffffffff [UNCERTAIN U-3922]; second is pointer to the 2-float pair.
    const std::uint32_t uVar1 = s_FUN_004671a0_2(
        0xffffffffu,
        reinterpret_cast<void*>(&local_8));

    // FUN_004c1c80(uVar1) — RwCameraSetViewWindow candidate (C2).
    s_FUN_004c1c80(uVar1);
}

RH_ScopedInstall(SetDefaultViewWindow, 0x00492e60);


// ---------------------------------------------------------------------------
// DataZeroFill  --  0x004924f0  --  DEFERRED
//
// DEFERRED: DataZeroFill requires harness extension for large-buffer pre/post
// state save. The function zero-fills 0xdce9 DWORDs (= 0x35da4 bytes = ~220 KB)
// starting at DAT_007f0f60, then runs complex nested init loops and calls 5
// depth-1 callees at C1 confidence.
//
// Reasons for deferral:
//  1. Buffer save/restore: 0x35da4 bytes of game state altered per call. A
//     synthetic A/B would corrupt MASHED.exe memory and crash on call #2 (after
//     orig already zeroed the region, reimpl zeros it again — identical, but the
//     subsequent nested loops write different data on each invocation because the
//     init state is already consumed).
//  2. 5 of 6 depth-1 callees are C1 (0x00431ae0, 0x00431af0, 0x00431b00,
//     0x0045b350, 0x00431b10). Anti-island rule: at least one callee must be C2+
//     to permit C3. Only FUN_00431d00 is C2; the other 5 are C1. This alone
//     blocks C3 until those callees are promoted.
//  3. [UNCERTAIN U-0009] structural: inner switch (cases 0–11) effective-address
//     math per-iteration not pinpointed. Blocks=none per UNCERTAINTIES.md, but
//     combined with (1) and (2) this is firmly deferred.
//
// Re-pickup condition: all 5 C1 callees promoted to C2+, AND a large-buffer
// save/restore arg_type or a fresh-state (boot-time) Frida scenario added.
// See DEFERRED.md for tracking row.
//
// Analysis: re/analysis/boot_app_init/004924f0.md (C2)
// ---------------------------------------------------------------------------
