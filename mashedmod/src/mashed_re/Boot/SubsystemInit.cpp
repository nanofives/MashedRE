// Mashed RE — Boot/SubsystemInit.cpp
// Boot subsystem init cluster: SubsystemInit, DisplayInit, ViewportInit,
// and SetDefaultViewWindow.
//
// Covers:
//   0x00492270  SubsystemInit          — 3-callee gate: RW_INIT_FN(0) + DisplayInit + ViewportInit
//   0x004921d0  DisplayInit            — display init: RW video-mode + LoadIcon TXD
//   0x00428590  ViewportInit           — MOVED to Match/MatchS06_00428590.cpp (MATCH-S06)
//   0x00492e60  SetDefaultViewWindow   — MOVED to Match/MatchS15_00492e60.cpp (MATCH-FIX-492E60)
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

// ---------------------------------------------------------------------------
// Globals written by DisplayInit — from analysis note 0x004921d0.md.
// ---------------------------------------------------------------------------
static void** const g_DAT_0077195c =
    reinterpret_cast<void**>(0x0077195cu);       // 0x00492255: global TXD ptr
static void** const g_DAT_00771960 =
    reinterpret_cast<void**>(0x00771960u);       // 0x00492269: global LoadIcon-texture ptr

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

// MATCH-S15: the body of 0x00492270 (SubsystemInit) moved to Match/MatchS15_00492270.cpp,
// which compiles the byte-matched source re/match/s15/00492270.c.


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

// MATCH-S15: the body of 0x004921d0 (DisplayInit) moved to Match/MatchS15_004921d0.cpp,
// which compiles the byte-matched source re/match/s15/004921d0.c.

// MATCH-S06: the body of 0x00428590 (ViewportInit) moved to Match/MatchS06_00428590.cpp,
// which compiles the byte-matched source re/match/s06/00428590.c.

// MATCH-FIX-492E60: the body of 0x00492e60 (SetDefaultViewWindow) moved to Match/MatchS15_00492e60.cpp,
// which compiles the byte-matched source re/match/s15/00492e60.c (install still mass-disabled there).


// ---------------------------------------------------------------------------
// DataZeroFill  --  0x004924f0  --  DEFERRED (stub passthrough)
//
// DEFERRED: DataZeroFill C3 promotion blocked by anti-island rule (5 of 6
// depth-1 callees at C1: 0x00431ae0, 0x00431af0, 0x00431b00, 0x0045b350,
// and 0x00431b10). Only FUN_00431d00 is C2. Callee promotions required first.
// Also blocked by [UNCERTAIN U-0009] inner-switch effective-address math.
//
// Harness-side unblocked by large_buffer_save_restore arg_type (added
// 2026-05-22 session C): harness now snapshots DAT_007f0f60 (0xdce9 dwords =
// ~220 KB) before each call pair and restores between orig and reimpl.
//
// This stub is a PASSTHROUGH to the original function so the .asi exports the
// symbol for run_diff_parallel. The actual reimplementation will replace this
// passthrough once callee promotions land.
//
// Full function body (from analysis 004924f0.md):
//   Zeroes 0xdce9 DWORDs at DAT_007f0f60 via REP STOSD.
//   Then runs a nested init loop (switch cases 0-11) calling 6 depth-1 callees.
// See re/analysis/boot_app_init/004924f0.md for full body.
//
// Re-pickup condition: all 5 C1 callees promoted to C2+.
// ---------------------------------------------------------------------------

#ifdef MASHED_STANDALONE
// [PORT-TOP-04D] exe: the body is the byte-matched source re/match/topm01/004924f0.c (464 B, relocheck OK), reached only
// from the matched WinMain 0x00492370 on the MASHED_ORIG_TOP path (Match/PortTop_00492370.cpp). Callees bind by name to
// their exe bodies; no raw-address call. The .asi keeps the passthrough below.
#define MATCH_PORT 1
#define FUN_004924f0 DataZeroFill
#define FUN_00431ae0 DefaultParam_SetField04   // exe body, Match/MatchS07_00431ae0.cpp
#define FUN_00431af0 DefaultParam_SetField08   // exe body, Match/MatchS07_00431af0.cpp
#define FUN_00431b00 DefaultParam_SetField00   // exe body, Match/MatchS07_00431b00.cpp
#define FUN_00431b10 BootParamSet2             // matched, Match/MatchS07_00431b10.cpp (exe since PORT-TOP-04D)
#define FUN_00431d00 CarSelectReset            // exe body, Match/MatchS07_00431d00.cpp
#define FUN_0045b350 RwInitNullStub            // exe body, Match/MatchS09_0045b350.cpp
extern "C" {
#include "../../../../re/match/topm01/004924f0.c"
}
#else
using DataZeroFill_t = void (__cdecl*)();
static DataZeroFill_t const s_orig_DataZeroFill =
    reinterpret_cast<DataZeroFill_t>(0x004924f0u);

// 0x004924f0
extern "C" __declspec(dllexport) void __cdecl DataZeroFill() {
    s_orig_DataZeroFill();
}
#endif

// MASS-DISABLED 2026-05-24 c3-boot-hangs: RH_ScopedInstall(DataZeroFill, 0x004924f0);
