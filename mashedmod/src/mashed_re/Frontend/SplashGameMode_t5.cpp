// Mashed RE - SplashGameMode_t5 cluster (c3-batch-t-s5).
//
// C2->C3 promotions for intro_splash + game_mode + race_results clusters.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// INCLUDED (GREEN diff candidates):
//   0x00493f70  VideoStateFlagGet      — 5B leaf: returns DAT_00771a04
//   0x00493fc0  AspectRatioGlobalGet   — 5B leaf: returns DAT_00771a18 (ignores stack)
//   0x00431d80  TiebreakFlagGet        — 5B leaf: returns DAT_0067ea7c
//   0x0046c700  EntityScoreFieldAdd    — 43B incrementer: DAT_008820b0[idx*0xd04] += p2
//   0x004c75e0  ViewportOriginGetter   — 27B: reads u16@+0x1c/+0x1e into out-ptrs
//   0x00494f30  AspectRatioSnapshot    — 15B: DAT_00771a50 = DAT_00771a54 = read()
//
// REFUSED (no viable safe diff harness):
//   0x004d8000  FUN_004d8000  — list traversal w/ 2 indirect callbacks per node;
//                               cannot synth-test without constructing the
//                               container list + valid callback pointers
//   0x00493fd0  FUN_00493fd0  — 630B render: vtable calls via DAT_007d3ff8,
//                               4-vertex quad draw; needs live render context
//   0x004c1be0  FUN_004c1be0  — calls FUN_004c7730 (STUB S-0820); HWND arg
//                               not safe to synth
//   0x00495080  FUN_00495080  — wrapper to FUN_00494fd0 (STUB S-3625); deep state
//
// Analysis notes:
//   re/analysis/skeleton_prep_boot_winmain_a/00493f70.md
//   re/analysis/intro_splash/0x00493fc0.md
//   re/analysis/race_results/00431d80.md
//   re/analysis/race_results/0046c700.md
//   re/analysis/intro_splash_d2/0x004c75e0.md
//   re/analysis/game_mode_cont2/0x00494f30.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ===========================================================================
// VideoStateFlagGet  --  0x00493f70
//
// Original: FUN_00493f70 (5 bytes, 0x00493f70..0x00493f74)
// Signature: undefined4 FUN_00493f70(void)
// Body (pure leaf):
//   eax = DAT_00771a04   // mov-load 4-byte global       [0x00493f70 body]
//   ret                                                  [0x00493f70 body]
//
// Constants (cited from 0x00493f70 body):
//   0x00771a04  — global; 4-byte read
//
// HOT-PATH NOTE (from analysis-note):
//   Fires ~170 calls/sec at the main menu. Frida Interceptor.attach unsafe
//   per CLAUDE.md — diff via run_diff.py (synthetic force-call) is fine,
//   but mass-canonical-observe Interceptor runs must exclude this RVA.
//
// ref: re/analysis/skeleton_prep_boot_winmain_a/00493f70.md
// ===========================================================================

// 0x00493f70
extern "C" __declspec(dllexport) std::uint32_t __cdecl VideoStateFlagGet(void) {
    // Read DAT_00771a04 (cited at 0x00493f70 body).
    return *reinterpret_cast<volatile std::uint32_t*>(0x00771a04u);
}

RH_ScopedInstall(VideoStateFlagGet, 0x00493f70);

// ===========================================================================
// AspectRatioGlobalGet  --  0x00493fc0
//
// Original: FUN_00493fc0 (5 bytes, 0x00493fc0..0x00493fc4)
// Signature: undefined4 FUN_00493fc0(void)  (decompiler sig)
// Body (pure leaf — 5-byte mov+ret):
//   eax = DAT_00771a18   // mov-load 4-byte global       [0x00493fc0 body]
//   ret                                                  [0x00493fc0 body]
//
// Note: callers (e.g. FUN_00494f30) push two float args before calling, but
// the 5-byte body ignores them entirely. Stack cleanup is the caller's
// responsibility (cdecl). Reimpl declares `void` arg list to match the body.
//
// Constants (cited from 0x00493fc0 body):
//   0x00771a18  — global; 4-byte read
//
// Uncertainties (carried, non-blocking for reimpl):
//   U-0814: caller pushes 2 floats but body ignores them — known.
//   U-0815: type of DAT_00771a18 (likely RW raster scale) — irrelevant here.
//
// ref: re/analysis/intro_splash/0x00493fc0.md
// ===========================================================================

// 0x00493fc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl AspectRatioGlobalGet(void) {
    // Read DAT_00771a18 (cited at 0x00493fc0 body).
    return *reinterpret_cast<volatile std::uint32_t*>(0x00771a18u);
}

RH_ScopedInstall(AspectRatioGlobalGet, 0x00493fc0);

// ===========================================================================
// TiebreakFlagGet  --  0x00431d80
//
// Original: FUN_00431d80 (5 bytes, 0x00431d80..0x00431d84)
// Signature: undefined4 FUN_00431d80(void)
// Body (pure leaf):
//   eax = DAT_0067ea7c   // mov-load 4-byte global       [0x00431d80 body]
//   ret                                                  [0x00431d80 body]
//
// Constants (cited from 0x00431d80 body):
//   0x0067ea7c  — global; tiebreak-enable flag in mode-4/code-6 path
//
// Uncertainty U-1305 (carried, non-blocking): write-site set of DAT_0067ea7c.
//
// ref: re/analysis/race_results/00431d80.md
// ===========================================================================

// 0x00431d80
extern "C" __declspec(dllexport) std::uint32_t __cdecl TiebreakFlagGet(void) {
    // Read DAT_0067ea7c (cited at 0x00431d80 body).
    return *reinterpret_cast<volatile std::uint32_t*>(0x0067ea7cu);
}

RH_ScopedInstall(TiebreakFlagGet, 0x00431d80);

// ===========================================================================
// EntityScoreFieldAdd  --  0x0046c700
//
// Original: FUN_0046c700 (43 bytes, 0x0046c700..0x0046c72a)
// Signature: undefined4 FUN_0046c700(int param_1, int param_2)
//   param_1: entity index (0..15)
//   param_2: integer delta to add
// Body:
//   if (param_1 > 0xf) return 0                          [0x0046c704]
//   *(int*)(0x008820b0 + param_1 * 0xd04) += param_2     [0x0046c717]
//   return 1                                              [0x0046c726]
//
// Constants (cited from 0x0046c700 body):
//   0x008820b0  — per-entity struct base (cited at 0x0046c700)
//   0xd04       — per-entity stride (3332 dec)            [0x0046c700]
//   0xf         — upper-bound check (15)                  [0x0046c704]
//
// Uncertainty U-1306 (carried, non-blocking): semantic of int@+0x0 of entity
// struct.
//
// ref: re/analysis/race_results/0046c700.md
// ===========================================================================

// 0x0046c700
extern "C" __declspec(dllexport) std::uint32_t __cdecl EntityScoreFieldAdd(int param_1, int param_2) {
    // Guard at 0x0046c704: param_1 > 15 -> return 0.
    if (param_1 > 0xf) {
        return 0u;
    }
    // Add param_2 to int at 0x008820b0 + param_1 * 0xd04 (cited at 0x0046c717).
    int* slot = reinterpret_cast<int*>(0x008820b0u + static_cast<std::uint32_t>(param_1) * 0xd04u);
    *slot += param_2;
    // Return 1 at 0x0046c726.
    return 1u;
}

RH_ScopedInstall(EntityScoreFieldAdd, 0x0046c700);

// ===========================================================================
// ViewportOriginGetter  --  0x004c75e0
//
// Original: FUN_004c75e0 (27 bytes, 0x004c75e0..0x004c75fa)
// Signature: void FUN_004c75e0(int param_1, undefined2* param_2, undefined2* param_3)
//   param_1: pointer to video object
//   param_2: out — short
//   param_3: out — short
// Body (pure leaf, no callees):
//   *param_2 = *(u16*)(param_1 + 0x1c)                   [0x004c75e0 body]
//   *param_3 = *(u16*)(param_1 + 0x1e)                   [0x004c75e0 body]
//   return
//
// Constants (cited from 0x004c75e0 body):
//   +0x1c (28) — u16 viewport-origin field 0
//   +0x1e (30) — u16 viewport-origin field 1
//
// Uncertainty U-2347 (carried, non-blocking): semantic of fields 0x1c/0x1e
// (FUN_004c77c0 initialises both to 0).
//
// ref: re/analysis/intro_splash_d2/0x004c75e0.md
// ===========================================================================

// 0x004c75e0
extern "C" __declspec(dllexport) void __cdecl ViewportOriginGetter(int param_1,
                                                                   std::uint16_t* param_2,
                                                                   std::uint16_t* param_3) {
    // Read u16 at param_1+0x1c into *param_2 (cited at 0x004c75e0 body).
    *param_2 = *reinterpret_cast<const std::uint16_t*>(param_1 + 0x1c);
    // Read u16 at param_1+0x1e into *param_3 (cited at 0x004c75e0 body).
    *param_3 = *reinterpret_cast<const std::uint16_t*>(param_1 + 0x1e);
}

RH_ScopedInstall(ViewportOriginGetter, 0x004c75e0);

// ===========================================================================
// AspectRatioSnapshot  --  0x00494f30
//
// Original: FUN_00494f30 (15 bytes, 0x00494f30..0x00494f3e)
// Signature: void FUN_00494f30(void)
// Body (calls one C2 callee — FUN_00493fc0, see above):
//   eax = FUN_00493fc0()             // returns DAT_00771a18 [0x00494f30 body]
//   DAT_00771a50 = eax                                       [0x00494f30 body]
//   DAT_00771a54 = eax                                       [0x00494f30 body]
//   return
//
// Effective semantics: snapshot DAT_00771a18 -> {DAT_00771a50, DAT_00771a54}.
//
// Constants (cited from 0x00494f30 body):
//   0x00771a18  — source global (read via FUN_00493fc0)
//   0x00771a50  — destination 1 (4-byte write)
//   0x00771a54  — destination 2 (4-byte write; same value)
//
// Uncertainty U-3648 (carried, non-blocking): semantic of the
// 0x00771a18/0x00771a50/0x00771a54 trio (snapshot for delta/interp).
//
// Callee FUN_00493fc0 is C2 (also being promoted in this batch via
// AspectRatioGlobalGet above). At runtime we forward through the original
// RVA so the reimpl matches the original's call sequence bit-for-bit.
//
// ref: re/analysis/game_mode_cont2/0x00494f30.md
// ===========================================================================

// 0x00493fc0  AspectRatioGlobalGet — forwarder to original.
static std::uint32_t OrigAspectRatioGlobalGet(void) {
    typedef std::uint32_t (__cdecl* Fn)(void);
    return reinterpret_cast<Fn>(static_cast<std::uintptr_t>(0x00493fc0u))();
}

// 0x00494f30
extern "C" __declspec(dllexport) void __cdecl AspectRatioSnapshot(void) {
    // Call FUN_00493fc0 (returns DAT_00771a18) -- cited at 0x00494f30 body.
    std::uint32_t v = OrigAspectRatioGlobalGet();
    // DAT_00771a50 = v -- cited at 0x00494f30 body.
    *reinterpret_cast<volatile std::uint32_t*>(0x00771a50u) = v;
    // DAT_00771a54 = v -- cited at 0x00494f30 body.
    *reinterpret_cast<volatile std::uint32_t*>(0x00771a54u) = v;
}

RH_ScopedInstall(AspectRatioSnapshot, 0x00494f30);
